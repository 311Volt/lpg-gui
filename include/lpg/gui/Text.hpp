#ifndef LPG_GUI_TEXT_H
#define LPG_GUI_TEXT_H

#include "Window.hpp"
#include "Math.hpp"
#include "../a5xx/Font.hpp"
#include "../a5xx/Bitmap.hpp"

namespace gui {
	class Text : public Window {
	public:
		Text();
		Text(float x, float y);
		~Text();

		static Point GetBitmapSize();

		virtual void onRescale() override;
		virtual void onTitleChange() override;
		virtual void render() override;
	private:
		std::string fontName;
		std::string text;
	};
}

#endif // LPG_GUI_TEXT_H
