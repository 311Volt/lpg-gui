#ifndef LPG_GUI_TEXT_H
#define LPG_GUI_TEXT_H

#include <lpg/gui/Window.hpp>

#include <axxegro/math/math.hpp>
#include <axxegro/resources/Font.hpp>
#include <axxegro/resources/Bitmap.hpp>

#include <lpg/resmgr/ResourceManager.hpp>

#include <variant>
#include <list>

namespace gui {
	class Text : public Window {
	public:
		Text();
		Text(float x, float y, float w, float h);
		virtual ~Text();

		virtual std::string_view className() {return "Text";};

		void setFont(const std::string& resName);

		virtual void onRescale() override;
		virtual void onTitleChange() override;
		virtual void render() override;

		lpg::ResourceID getFontID();

		constexpr static char32_t TXT_FMT_CHAR = 0x00A7;

		enum class SizeMode: uint8_t {
			NORMAL = 0,
			AUTO = 1
		};
		
		void update();
		void setSizeMode(SizeMode mode);

		/**
		 * available formatting tokens:
		 * FMT_CHAR+h - long color (0xRRGGBB)
		 * FMT_CHAR+s - short color (0xRGB)
		 * FMT_CHAR+r"ResourceName" - switch font
		 * FMT_CHAR+0 - reset color to default
		 * FMT_CHAR+z - reset font resource to default
		 */

		struct FmtToken {
			std::variant<al::Color, lpg::ResourceID> dat;
			uint32_t length;
		};

		struct RenderChunk {
			int lineNumber;
			std::string u8text;
			al::Color color;
			al::Vec2<int> pos;
			al::Rect<int> textDimensions;
			lpg::ResourceID rID;
		};
		
		void setText(const std::string_view text);
	protected:
		std::u32string buffer;
	private:
		std::optional<FmtToken> tryParseToken(const std::u32string_view tok);
		void buildRenderChunkList();
		void autoResize();
		std::vector<RenderChunk> renderChunks;
		
		SizeMode sizeMode;


		lpg::ResourceID rID;
		std::string text;
	};
}

#endif // LPG_GUI_TEXT_H
