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
		~Text();

		virtual std::string_view className() {return "Text";};

		void setFont(const std::string& resName);

		virtual void onRescale() override;
		virtual void onTitleChange() override;
		virtual void render() override;

		lpg::ResourceManager::ResourceID getFontID();

		constexpr static char32_t TXT_FMT_CHAR = 0x00A7;

		
		/**
		 * available formatting tokens:
		 * FMT_CHAR+h - long color (0xRRGGBB)
		 * FMT_CHAR+s - short color (0xRGB)
		 * FMT_CHAR+r"ResourceName" - switch font
		 * FMT_CHAR+0 - reset color to default
		 * FMT_CHAR+z - reset font resource to default
		 */

		struct FmtToken {
			std::variant<al::Color, lpg::ResourceManager::ResourceID> dat;
			uint32_t length;
		};

		struct RenderChunk {
			int lineNumber;
			std::string u8text;
			al::Color color;
			al::Vec2<int> pos;
			lpg::ResourceManager::ResourceID rID;
		};

		void setText(const std::u32string_view text);

	private:
		std::u32string buffer;
		std::optional<FmtToken> tryParseToken(const std::u32string_view tok);
		std::vector<RenderChunk> buildRenderChunkList();



		lpg::ResourceManager::ResourceID rID;
		std::string text;
	};
}

#endif // LPG_GUI_TEXT_H
