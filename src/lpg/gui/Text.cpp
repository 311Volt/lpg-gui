#include <lpg/gui/Text.hpp>
#include <lpg/util/Log.hpp>

#include <algorithm>

#include <fmt/format.h>

#include <allegro5/allegro_primitives.h>


gui::Text::Text(float x, float y, float w, float h)
	: Window(w, h, x, y)
{
	setFont("DefaultFont");
	setTitle("");
	enablePrerendering();
	
}

gui::Text::Text()
	: Text(0, 0, 90, 20)
{

}


gui::Text::~Text()
{

}

void gui::Text::setFont(const std::string& resName)
{
	rID = RM.getIdOf(resName);
}

void gui::Text::onRescale()
{
	Window::onRescale();
	onTitleChange();
}

lpg::ResourceManager::ResourceID gui::Text::getFontID()
{
	return rID;
}


void gui::Text::onTitleChange()
{
	std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
	
	al::Rect<int> td = font->getTextDimensions(title);
	td.b += {1, 1};
	td.b.x = std::max(td.a.x+5.0f, td.b.x+0.0f);
	td.b.y = std::max(td.a.y+5.0f, td.b.y+0.0f);

	//resize(ToUnits(td.size()));
}


void gui::Text::setText(std::u32string_view text)
{
	buffer = std::u32string(text.begin(), text.end());
}

int hexNum(char32_t c)
{
	if(c >= '0' && c <= '9') {
		return c - '0';
	} else if(c >= 'A' && c <= 'Z') {
		return 10 + c - 'A';
	} else if(c >= 'a' && c <= 'z') {
		return 10 + c - 'a';
	}
	return -1;
}

std::optional<uint32_t> parseHexNumber(const std::u32string_view str, uint32_t len)
{
	uint32_t ret = 0, i;
	for(i=0; i<str.size() && i<len; i++) {
		auto k = hexNum(str[i]);
		if(k >= 0) {
			ret *= 0x10;
			ret += k;
		} else {
			return std::nullopt;
		}
	}
	if(i < len) {
		return std::nullopt;
	}
	return ret;
}

std::optional<gui::Text::FmtToken> tryParseHexColorToken(const std::u32string_view cmd)
{
	auto colCode = parseHexNumber(cmd, 6);
	if(!colCode.has_value()) {
		return std::nullopt;
	}
	return gui::Text::FmtToken{al::Color::U32_RGB(colCode.value()), 6+2};
}


std::optional<gui::Text::FmtToken> tryParseShortHexColorToken(const std::u32string_view cmd)
{
	auto colCode = parseHexNumber(cmd, 3);
	if(!colCode.has_value()) {
		return std::nullopt;
	}

	auto v = colCode.value();
	uint32_t fullColCode (
		((v & 0xF00) << 12) +
		((v & 0x0F0) << 8) +
		((v & 0x00F) << 4)
	);
	return gui::Text::FmtToken{al::Color::U32_RGB(v), 3+2};
}


std::optional<gui::Text::FmtToken> tryParseResourceName(const std::u32string_view cmd)
{
	uint32_t i = 0;
	auto quote = std::find(cmd.begin(), cmd.end(), '"');
	std::string rName = al::UStr::EncodeToUTF8(std::u32string(cmd.begin(), quote));
	return gui::Text::FmtToken{gui::Window::RM.getIdOf(rName), uint32_t(quote-cmd.begin())};
}


std::optional<gui::Text::FmtToken> gui::Text::tryParseToken(const std::u32string_view tok)
{
	if(tok[0] != TXT_FMT_CHAR || tok.size() < 2) {
		return std::nullopt;
	}
	auto cmd = tok.at(1);
	switch(cmd) {
		case 'h': return tryParseHexColorToken(tok.substr(2));
		case 's': return tryParseShortHexColorToken(tok.substr(2));
		case '"': return tryParseResourceName(tok.substr(2));
		case '0': return FmtToken{textColor, 2};
		case 'z': return FmtToken{RM.getIdOf("DefaultFont"), 2};
	}

	return std::nullopt;
}
std::vector<gui::Text::RenderChunk> gui::Text::buildRenderChunkList()
{
	RenderChunk state;
	state.lineNumber = 0;
	state.u8text = "";
	state.pos = {0, 0};
	state.color = textColor;
	state.rID = rID;

	int x = 0;

	uint32_t currentLineHeight = 0;

	std::vector<RenderChunk> chunks;

	auto writeChunk = [&](){
		chunks.push_back(state);
		state.u8text = "";
		state.pos.x = x;
	};

	const auto [scrW, scrH] = getScreenSize();

	for(uint32_t i=0; i<buffer.size(); i++) {
		if(buffer[i] == TXT_FMT_CHAR) {
			auto tok = tryParseToken(std::u32string_view(buffer).substr(i));
			writeChunk();

			if(!tok.has_value()) {
				continue;
			}
			if(const al::Color* col = std::get_if<al::Color>(&tok->dat)) {
				state.color = *col;
			}
			using TRID = lpg::ResourceManager::ResourceID;
			if(const TRID* rID = std::get_if<TRID>(&tok->dat)) {
				state.rID = *rID;
			}
			
			i += tok->length;
			if(i > buffer.size()) {
				break;
			}
		}

		if(state.pos.y > scrH) {
			break;
		}
		
		auto font = RM.get<al::Font>(state.rID);
		currentLineHeight = std::max(currentLineHeight, (decltype(currentLineHeight))font->getLineHeight());
		int chrWidth = font->getGlyphAdvance(buffer[i], buffer[i+1] ? buffer[i+1] : ALLEGRO_NO_KERNING);

		if(buffer[i] == '\n' || state.pos.x + chrWidth > scrW) {
			//new line
			writeChunk();
			x = 0;
			state.pos.x = 0;
			state.pos.y += currentLineHeight;
			currentLineHeight = 1;
		} else {	
			state.u8text += al::UStr::EncodeToUTF8(std::u32string_view(buffer).substr(i,1));
			x += chrWidth;
		}
	}
	if(state.u8text.size()) {
		writeChunk();
	}
	return chunks;
}



void gui::Text::render()
{
	/*
	std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
	if(title.length()) {
		al::Rect<int> td = font->getTextDimensions(title);
		font->draw(title, textColor, -td.a);
	}
	*/
	//new rendering
	auto chunks = buildRenderChunkList();
	fmt::print("buffer: {}\n", al::UStr::EncodeToUTF8(buffer));
	fmt::print("number of chunks: {}\n", chunks.size());
	for(const auto& ch: chunks) {
		fmt::print("chunk: {} @ {:06X} ({},{})\n", ch.u8text, ch.color.rgb_u32(), ch.pos.x, ch.pos.y);
		auto font = RM.get<al::Font>(ch.rID);
		font->draw(ch.u8text, ch.color, ch.pos);
	}

	drawChildren();
}
