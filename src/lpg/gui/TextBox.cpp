#include <lpg/gui/TextBox.hpp>

#include <algorithm>
#include <axxegro/UStr.hpp>

gui::TextBox::TextBox(int x, int y, int w, int h)
	: Window(w, h, x, y)
{
	txt.setAlignment(Alignment::LEFT_CENTER);
	txt.setPos({4,0});
	addChild(txt);
	addChild(cursor);

	cursorPos = 0;

	buffer = U"Sample Text";
	updateText();
	
	cursor.setTitle("|");
	
	registerEventHandler(ALLEGRO_EVENT_KEY_CHAR, &TextBox::onKeyChar);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &TextBox::onMouseDown);
	
	setEdgeType(EdgeType::BEVELED_INWARD);
	setBgColor(al::Color::RGB(255,255,255));
}

void gui::TextBox::updateText()
{
	txt.setTitle(al::UStr::EncodeToUTF8(buffer));
}

void gui::TextBox::insertCharacter(int position, int32_t codepoint)
{
	buffer.insert(position, {(char32_t)codepoint});
	updateText();
}

void gui::TextBox::deleteCharacter(int position)
{
	if(position >= 0 && position <= (int)buffer.size()) {
		buffer.erase(position, 1);
	}
	updateText();
	
}

void gui::TextBox::onMouseDown(const ALLEGRO_EVENT& ev)
{
	focused = IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle());
	setBgColor(al::Color::RGB(255-int(focused)*40,255,255));
}


void gui::TextBox::onKeyChar(const ALLEGRO_EVENT& ev)
{
	if(isFocused()) {
		if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
			deleteCharacter(buffer.size()-1);
		} else if(ev.keyboard.unichar) {
			insertCharacter(buffer.size(), ev.keyboard.unichar);
		}
	}
}
