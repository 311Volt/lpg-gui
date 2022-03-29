#ifndef LPG_GUI_TEXTBOX_H
#define LPG_GUI_TEXTBOX_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Text.hpp>

namespace gui {
	class TextBox: public Window {
	public:
		TextBox(int x, int y, int w, int h);
		
		virtual std::string_view className() {return "TextBox";};
	private:
		Text txt;
		Text cursor;
		
		int cursorPos();
		
		void insertCharacter(int position, int32_t codepoint);
		void deleteCharacter(int position);
		
		void onKeyChar(const ALLEGRO_EVENT& ev);
		void onMouseDown(const ALLEGRO_EVENT& ev);
	};
}

#endif // LPG_GUI_TEXTBOX_H
