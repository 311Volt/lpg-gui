#ifndef LPG_GUI_TEXTBOX_H
#define LPG_GUI_TEXTBOX_H

#include "Window.hpp"
#include "Text.hpp"

namespace gui {
	class TextBox: public Window {
	public:
		TextBox(int x, int y, int w, int h);
		
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
