#ifndef LPG_GUI_TEXTBOX_H
#define LPG_GUI_TEXTBOX_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Text.hpp>

#include <string>

namespace gui {
	class TextBox: public Window {
	public:
		TextBox(int x, int y, int w, int h);
		
		std::string getText();
		void setText(const std::string_view txt);
		
		virtual std::string_view className() {return "TextBox";};
	private:
		std::u32string buffer;
		int viewPos;
		int cursorPos;
		Text txt;
		Text cursor;
		
		void updateText();

		

		void insertCharacter(int position, int32_t codepoint);
		void deleteCharacter(int position);
		
		void onKeyChar(const ALLEGRO_EVENT& ev);
		void onMouseDown(const ALLEGRO_EVENT& ev);

		virtual void render() override;
	};
}

#endif // LPG_GUI_TEXTBOX_H
