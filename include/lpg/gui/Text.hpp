#ifndef INCLUDE_LPG_GUI_TEXT
#define INCLUDE_LPG_GUI_TEXT

#include <lpg/gui/Window.hpp>

#include <axxegro/math/math.hpp>
#include <axxegro/resources/Font.hpp>
#include <axxegro/resources/Bitmap.hpp>

#include <lpg/resmgr/ResourceManager.hpp>

#include <variant>


/**
	* available formatting tokens:
	* FMT_CHAR+h - long color (0xRRGGBB)
	* FMT_CHAR+s - short color (0xRGB)
	* FMT_CHAR+r"ResourceName" - switch font
	* FMT_CHAR+0 - reset color to default
	* FMT_CHAR+z - reset font resource to default
*/


namespace gui {
	class Text : public Window {
	public:
		Text();
		Text(const al::Vec2<> size, const al::Vec2<> pos, const std::string_view text = "");

		LPG_WIN_CLS_NAME("Text");

		Text& setText(const std::string_view buffer);
		Text& setDefaultFont(const std::string& resourceName);
		//text color can be set with Window::setColor()

		std::string getText();

		void resizeToFit();
		al::Rect<> getSpan() const;

		al::Rect<> getPadding() const;
		al::Rect<> getTextRegion() const;
		Text& setPadding(const al::Rect<> padding);

		virtual void onTitleChange() override;
		virtual void render() override;
		virtual void onResize() override;

		constexpr static char32_t TXT_FMT_CHAR = 0x00A7; //section sign
	
		enum class TokenType: uint8_t {
			TEXT,
			COLOR_LONG,
			COLOR_SHORT,
			FONT,
			RESET_COLOR,
			RESET_FONT
		};
		struct RenderChunk {
			std::string u8text;
			al::Rect<> region;
			lpg::ResourceID fontId;
			al::Color color;
		};

		struct Token {
			TokenType type;
			size_t length;
			std::variant<al::Color, lpg::ResourceID, std::monostate> data = std::monostate();	
		};
		
		Alignment textAlignment;
		using LineRange = std::pair<size_t,size_t>;
	private:
		void update();

		Token parseToken(const std::u32string_view v);

		lpg::ResourceID defaultFontId;
		al::Rect<> padding;
		std::vector<Token> tokenize();
		std::vector<RenderChunk> buildRenderChunksFor(std::u32string_view txt, al::Color color, lpg::ResourceID fontId);
		std::vector<RenderChunk> buildRenderChunks(const std::vector<Token>& tokens);
		std::unordered_map<lpg::ResourceID, std::shared_ptr<al::Font>> getFonts(const std::vector<RenderChunk>& chunks);
		std::vector<LineRange> findLineRanges(const std::vector<RenderChunk>& chunks);
		void setChunkPositions(std::vector<RenderChunk>& chunks, const std::vector<LineRange>& lines);


		std::vector<RenderChunk> currentRenderChunks;
		std::u32string buffer;
	};
}

#endif /* INCLUDE_LPG_GUI_TEXT */
