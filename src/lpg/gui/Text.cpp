#include <lpg/gui/Text.hpp>
#include <lpg/util/Log.hpp>

#include <algorithm>
#include <numeric>

#include <fmt/format.h>




gui::Text::Text(const al::Vec2<> size, const al::Vec2<> pos, const std::string_view text)
	: Window(size, pos)
{
	textColor = al::Black;
	setBgColor(al::RGBA(0,0,0,0));
	setDefaultFont("DefaultFont");
	setEdgeType(EDGE_NONE);
	enablePrerendering();
	textAlignment = ALIGN_LEFT_TOP;
	setText(text);
	setPadding({3,3,3,3});

	needsUpdate = false;
}

gui::Text::Text()
	: Text({90,20}, {0,0})
{

}

gui::Text& gui::Text::setText(const std::string_view buffer)
{
	this->buffer = al::ToUTF32(buffer);
	needsUpdate = true;
	return *this;
}

gui::Text& gui::Text::setDefaultFont(const std::string& resourceName)
{
	defaultFontId = RM.getIdOf(resourceName);
	return *this;
}

std::string gui::Text::getText()
{
	return al::ToUTF8(buffer);
}

gui::Text& gui::Text::setPadding(const al::Rect<> padding)
{
	this->padding = padding;
	needsUpdate = true;
	return *this;
}

void gui::Text::resizeToFit()
{
	resize(ToUnits(al::CurrentDisplay.size()));
	update();
	if(currentRenderChunks.size() == 0) {
		resize({5,5});
	}
	resize(getSpan().size() + al::Vec2<>(4,4));
	needsUpdate = true;
}
al::Rect<> gui::Text::getSpan() const
{
	al::Rect<> ret;

	if(currentRenderChunks.size()) {
		ret = currentRenderChunks[0].region;
		for(auto& ch: currentRenderChunks) {
			ret = ret.makeUnion(ch.region);
		}
	}
	return ret;
}
void gui::Text::setTextAlignment(Alignment alignment)
{
	textAlignment = alignment;
	needsUpdate = true;
}
gui::Window::Alignment gui::Text::getTextAlignment()
{
	return textAlignment;
}

void gui::Text::setTextColor(al::Color color)
{
	textColor = color;
	needsUpdate = true;
}

void gui::Text::update()
{
	currentRenderChunks = buildRenderChunks(tokenize());
	needsRedraw = true;
}
void gui::Text::updateIfNeeded()
{
	if(needsUpdate) {
		update();
		needsUpdate = false;
	}
}

void gui::Text::onTitleChange()
{
	needsUpdate = true;
}

void gui::Text::onResize()
{
	needsUpdate = true;
}

void gui::Text::tick()
{
	if(needsUpdate) {
		needsUpdate = false;
		update();
	}
}

void gui::Text::render()
{
	Window::render();
	
	auto fonts = getFonts(currentRenderChunks);

	for(const auto& chunk: currentRenderChunks) {
		const auto& fon = fonts[chunk.fontId];
		fon->draw(chunk.u8text, chunk.color, chunk.region.a);
	}
}


int hexNum(char x)
{
	x = std::tolower(x);
	if(x >= 'a' && x <= 'f') {
		return 10 + x - 'a';
	} else if(x >= '0' && x <= '9') {
		return x - '0';
	} else {
		return -1;
	}
}

std::optional<uint32_t> hexValue(const std::u32string_view v, unsigned minSize)
{
	if(v.size() < minSize) {
		return std::nullopt;
	}
	uint32_t ret = 0;
	for(auto& ch: v) {
		if(ch > 0x7F) {
			return std::nullopt;
		}
		auto n = hexNum(ch);
		if(n < 0) {
			return std::nullopt;
		}
		ret = ret*0x10 + n;
	}
	return ret;
}

gui::Text::Token TokHexColor(const std::u32string_view v)
{
	if(auto val = hexValue(v.substr(2,6), 6)) {
		return {gui::Text::TokenType::COLOR_LONG, 8, al::U32_RGB(val.value())};
	}
	return {gui::Text::TokenType::TEXT, 8};
}

gui::Text::Token TokHexColorShort(const std::u32string_view v)
{
	if(auto val = hexValue(v.substr(2,3), 3)) {
		uint32_t shortValue = val.value();
		uint32_t realValue = 
			((shortValue & 0x00F) << 4) +
			((shortValue & 0x0F0) << 8) +
			((shortValue & 0xF00) << 12)
		;
		return {gui::Text::TokenType::COLOR_LONG, 5, al::U32_RGB(realValue)};
	}
	return {gui::Text::TokenType::TEXT, 5};
}

gui::Text::Token TokFont(const std::u32string_view v)
{
	if(v.size() > 4 && v.at(2) == '"') {
		auto nameBegin = v.begin()+3;
		auto closingQuote = std::find(nameBegin, v.end(), '"');

		if(closingQuote == v.end()) {  //refuse to parse the rest to prevent quadratic behavior 
			return {gui::Text::TokenType::TEXT, v.size()};
		}
		
		auto resourceName32 = v.substr(3, std::distance(nameBegin, closingQuote));
		auto resourceName = al::ToUTF8(resourceName32);
		auto rID = gui::Window::RM.getIdOf(resourceName);

		return {gui::Text::TokenType::FONT, (size_t)std::distance(v.begin(), closingQuote+1), rID};
	}
	return {gui::Text::TokenType::TEXT, 1};
}

gui::Text::Token gui::Text::parseToken(const std::u32string_view v)
{
#ifndef NDEBUG
	if(v.size() == 0) {
		throw std::invalid_argument("an empty view should never be passed here");
	}
#endif
	// if v begins with the format character, try to parse the token
	if(v[0] == TXT_FMT_CHAR && v.size() > 1) {
		switch(v.at(1)) {
			case 'h': return TokHexColor(v);
			case 's': return TokHexColorShort(v);
			case 'r': return TokFont(v);
			case '0': return {TokenType::RESET_COLOR, 2, textColor};
			case 'z': return {TokenType::RESET_FONT, 2, defaultFontId};
			default: ;
		}
	}

	// else, search for either the fmt char or the end of the buffer
	// and treat everything up to that point as normal text
	auto ch = std::find(v.begin(), v.end(), TXT_FMT_CHAR);
	size_t npos = std::distance(v.begin(), ch);
	return {TokenType::TEXT, npos};
}

std::vector<gui::Text::Token> gui::Text::tokenize()
{
	std::vector<gui::Text::Token> ret;
	for(size_t i=0; i<buffer.size(); ) {
		ret.push_back(parseToken(buffer.substr(i)));
		i += std::max<size_t>(1, ret.back().length);
	}
	return ret;
}


al::Rect<> gui::Text::getPadding() const
{
	return padding;
}

al::Rect<> gui::Text::getTextRegion() const
{
	al::Rect<> r = {{0,0}, getSize()};
	return {
		r.a + padding.a,
		r.b - padding.b
	};
}

bool isspace32(char32_t c)
{
	if(c >= 128)
		return false;
	return std::isspace(c);
}

std::vector<gui::Text::RenderChunk> gui::Text::buildRenderChunksFor(
	std::u32string_view txt, 
	al::Color color, 
	lpg::ResourceID fontId
)
{
	std::vector<RenderChunk> ret;

	while(txt.size()) {
		auto nextWord = std::find_if(txt.begin(), txt.end(), isspace32);
		while(nextWord != txt.end() && isspace32(*nextWord)) {
			++nextWord;
		}

		auto nextWordOffset = std::distance(txt.begin(), nextWord);
		std::u32string_view currWord = txt.substr(0, nextWordOffset);

		RenderChunk ch;
		ch.color = color;
		ch.fontId = fontId;
		ch.region = {-1, -1, 0, 0};
		ch.u8text = al::ToUTF8(currWord);

		ret.push_back(ch);
		txt = txt.substr(nextWordOffset);
	}
	return ret;
}

std::unordered_map<lpg::ResourceID, std::shared_ptr<al::Font>> gui::Text::getFonts(const std::vector<RenderChunk>& chunks)
{
	std::unordered_map<lpg::ResourceID, std::shared_ptr<al::Font>> ret;

	for(auto& ch: chunks) {
		if(ret.count(ch.fontId) == 0) {
			ret[ch.fontId] = RM.get<al::Font>(ch.fontId);
		}
	}

	return ret;
}

std::vector<gui::Text::LineRange> IndicesToRanges(const std::vector<size_t>& v)
{
	if(!v.size()) {
		return {};
	}

	std::vector<gui::Text::LineRange> ret;
	size_t last = v[0];
	size_t lastChange = 0;
	for(size_t i=0; i<v.size(); i++) {
		while(last < v[i]) {
			ret.push_back({lastChange, i});
			lastChange = i;
			++last;
		}
	}
	ret.push_back({lastChange, v.size()});
	return ret;
}

std::vector<gui::Text::LineRange> gui::Text::findLineRanges(const std::vector<RenderChunk>& chunks)
{
	std::vector<size_t> lineNumbers(chunks.size());
	auto fonts = getFonts(chunks);
	auto region = getTextRegion();

	float currentWidth = 0.0;

	int lineNumber = 0;

	for(size_t i=0; i<chunks.size(); i++) {
		const auto& ch = chunks[i];
		int chunkWidth = fonts[ch.fontId]->getTextWidth(ch.u8text);

		size_t numNewlines = std::count(ch.u8text.begin(), ch.u8text.end(), '\n');

		currentWidth += chunkWidth;
		if(currentWidth > region.width()) {
			currentWidth = std::min<float>(chunkWidth, ToPixels(region.width()));
			++lineNumber;
		}
		lineNumbers[i] = lineNumber;
		lineNumber += numNewlines;

		if(numNewlines) {
			currentWidth = 0.0;
		}
	}

	return IndicesToRanges(lineNumbers);
}

void gui::Text::setChunkPositions(std::vector<RenderChunk>& chunks, const std::vector<LineRange>& lines)
{
	auto fonts = getFonts(chunks);

	lpg::ResourceID currentFontId = chunks.at(0).fontId;

	auto region = ToPixels(this->getTextRegion());
	auto currPos = region.topLeft();

	float yMax = region.a.y;

	for(const auto& line: lines) {
		int maxAscent = 0;
		int maxDescent = 0;

		int maxLineHeight = 0;
		if(line.second - line.first == 0) {
			maxLineHeight = fonts[currentFontId]->getLineHeight();
		}

		currPos.x = region.a.x;

		//1st pass
		for(size_t i=line.first; i<line.second; i++) {
			const auto& ch = chunks[i];
			const auto& fon = fonts[ch.fontId];

			maxLineHeight = std::max<int>(maxLineHeight, fon->getLineHeight());
			maxAscent = std::max<int>(maxAscent, fon->getAscent());
			maxDescent = std::max<int>(maxDescent, fon->getDescent());
		}

		//2nd pass
		for(size_t i=line.first; i<line.second; i++) {
			auto& ch = chunks[i];
			const auto& fon = fonts[ch.fontId];
			currentFontId = ch.fontId;
			auto ascent = fon->getAscent();
			auto offset = currPos + al::Vec2<>(0, maxAscent-ascent-maxDescent);
			ch.region = al::Rect<>::PosSize(offset, fon->getTextDimensions(ch.u8text).size());

			currPos.x += fon->getTextWidth(ch.u8text);
		}

		//3rd pass: alignment_x
		float xMax = chunks[line.first].region.b.x;
		
		for(size_t i=line.first; i<line.second; i++) {
			const auto& ch = chunks[i];
			xMax = std::max<float>(xMax, ch.region.b.x);
			yMax = std::max<float>(yMax, ch.region.b.y);
		}

		float xOff = (region.b.x - xMax) * ((((int)textAlignment & 0b1100) >> 2) / 2.0);
		for(size_t i=line.first; i<line.second; i++) {
			chunks[i].region += al::Vec2<>(xOff, 0);
		}

		currPos.y += maxLineHeight;
	}
	
	float yOff = (region.b.y - yMax) * (((int)textAlignment & 0b0011) / 2.0);
	for(auto& ch: chunks) {
		ch.region += al::Vec2<>(0, yOff);
	}
}

std::vector<gui::Text::RenderChunk> gui::Text::buildRenderChunks(const std::vector<Token>& tokens)
{
	std::vector<RenderChunk> ret;
	al::Color currentColor = this->textColor;
	lpg::ResourceID currentFontID = defaultFontId;

	size_t tPos = 0;
	for(const auto& tok: tokens) {
		const auto tokText = buffer.substr(tPos, tok.length);
		tPos += tok.length;
		if(std::holds_alternative<lpg::ResourceID>(tok.data)) {
			currentFontID = std::get<lpg::ResourceID>(tok.data);
		} else if(std::holds_alternative<al::Color>(tok.data)) {
			currentColor = std::get<al::Color>(tok.data);
		} else if(tok.type == TokenType::TEXT) {
			auto v = buildRenderChunksFor(tokText, currentColor, currentFontID);
			for(auto& ch: v) {
				ret.push_back(ch);
			}
		}
	}
	if(ret.size()) {
		auto lines = findLineRanges(ret);
		setChunkPositions(ret, lines);
	}

	for(auto& ch: ret) {
		while(ch.u8text.size() && std::isspace(ch.u8text.back())) {
			ch.u8text.pop_back();
		}
	}

	return ret;
}