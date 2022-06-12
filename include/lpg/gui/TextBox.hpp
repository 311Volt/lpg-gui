#ifndef LPG_GUI_TEXTBOX_H
#define LPG_GUI_TEXTBOX_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Text.hpp>

#include <string>

namespace gui {
	class TextBox: public Window {
	public:
		TextBox(al::Vec2<> size, al::Vec2<> pos, const std::string& initialText = "");
		
		std::string getText();
		void setText(const std::string_view txt);

		void setOnReturnCallback(std::function<void(void)> fn);
		
		LPG_WIN_CLS_NAME("TextBox");
	
	protected:
		virtual void updateText();
		Text txt;
		std::u32string buffer;

		std::function<void(void)> onReturn;
		int viewPos;
		int cursorPos;
		Text cursor;

		void insertCharacter(int position, int32_t codepoint);
		void deleteCharacter(int position);
		
		void onKeyChar(const ALLEGRO_EVENT& ev);
		void onMouseDown(const ALLEGRO_EVENT& ev);

		virtual void tick() override;
		virtual void render() override;
	};
}

#endif // LPG_GUI_TEXTBOX_H
