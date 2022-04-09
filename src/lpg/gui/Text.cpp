#include <lpg/gui/Text.hpp>
#include <lpg/util/Log.hpp>

#include <algorithm>
#include <numeric>

#include <fmt/format.h>

#include <allegro5/allegro_primitives.h>


gui::Text::Text(float x, float y, float w, float h)
	: Window(w, h, x, y)
{
	setSizeMode(SizeMode::NORMAL);
	setFont("DefaultFont");
	setTitle("");
	enablePrerendering();
	
	setBgColor(al::Color::RGBA(0,0,0,0));
	setEdgeType(EdgeType::NONE);
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
	update();
}

lpg::ResourceID gui::Text::getFontID()
{
	return rID;
}

void gui::Text::update()
{
	if(sizeMode == SizeMode::AUTO && parent) {
		resize(parent->getSize());
	}
	buildRenderChunkList();
	if(sizeMode == SizeMode::AUTO) {
		autoResize();
	}
	needsRedraw = true;
}

void gui::Text::autoResize()
{
	if(renderChunks.size() == 0) {
		return;
	}
	al::Rect<int> span = renderChunks[0].textDimensions;
	for(auto& ch: renderChunks) {
		span = span.makeUnion(ch.textDimensions);
	}
	
	span.a -= al::Vec2<int>(1,1);
	span.b += al::Vec2<int>(1,1);

	span.limit({2, 2});

	this->resize(ToUnits(span.size()));
}

void gui::Text::onTitleChange()
{
	//update();
}


void gui::Text::setText(const std::string_view text)
{
	buffer = al::UStr::DecodeToUTF32(text);
	update();
}

void gui::Text::setSizeMode(gui::Text::SizeMode mode)
{
	sizeMode = mode;
	if(mode == SizeMode::AUTO) {
		update();
	}
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
	return gui::Text::FmtToken{al::Color::U32_RGB(fullColCode), 3+2};
}


std::optional<gui::Text::FmtToken> tryParseResourceName(const std::u32string_view cmd)
{
	auto quote = std::find(cmd.begin(), cmd.end(), '"');
	std::string rName = al::UStr::EncodeToUTF8(std::u32string(cmd.begin(), quote));
	return gui::Text::FmtToken{gui::Window::RM.getIdOf(rName), 3+uint32_t(quote-cmd.begin())};
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
void gui::Text::buildRenderChunkList()
{
	renderChunks.clear();
	RenderChunk state;
	state.lineNumber = 0;
	state.u8text = "";
	state.pos = {0, 0};
	state.color = textColor;
	state.rID = rID;

	int x = 0;

	uint32_t currentLineHeight = 0;

	auto writeChunk = [&](){
		state.textDimensions = RM.get<al::Font>(state.rID)->getTextDimensions(state.u8text);
		renderChunks.push_back(state);
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
			if(const lpg::ResourceID* rID = std::get_if<lpg::ResourceID>(&tok->dat)) {
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

		int32_t nextChr = ALLEGRO_NO_KERNING;
		if(i < buffer.size() - 1) {
			nextChr = buffer[i+1];
		}
		int chrWidth = font->getGlyphAdvance(buffer[i], nextChr);


		if(buffer[i] == '\n' || state.pos.x + chrWidth > scrW) {
			//new line
			writeChunk();

			x = 0;
			state.lineNumber++;
			state.pos.x = 0;
			state.pos.y += currentLineHeight;
			currentLineHeight = 1;
		} else {	
			state.u8text += al::UStr::EncodeToUTF8(std::u32string_view(buffer).substr(i,1));
			x += chrWidth;
		}
		fmt::print("\n");
	}
	if(state.u8text.size()) {
		writeChunk();
	}
}



void gui::Text::render()
{
	Window::render();

	for(const auto& ch: renderChunks) {
		auto font = RM.get<al::Font>(ch.rID);
		font->draw(ch.u8text, ch.color, ch.pos - ch.textDimensions.a);
	}

	drawChildren();
}
