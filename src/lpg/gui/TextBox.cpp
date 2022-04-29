#include <lpg/gui/TextBox.hpp>

#include <algorithm>
#include <axxegro/UStr.hpp>

gui::TextBox::TextBox(int x, int y, int w, int h)
	: Window(w, h, x, y)
{
	txt.resize(this->dims);
	cursor.resize(this->dims);

	txt.setSizeMode(Text::SizeMode::AUTO);
	txt.setAlignment(Alignment::LEFT_CENTER);
	txt.setPos({4,0});
	addChild(txt);
	addChild(cursor);

	viewPos = 0;
	cursorPos = 0;

	buffer = U"Sample Text";
	updateText();
	
	cursor.setText("|");
	cursor.visible = false;
	
	registerEventHandler(ALLEGRO_EVENT_KEY_CHAR, &TextBox::onKeyChar);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &TextBox::onMouseDown);
	
	setEdgeType(EdgeType::BEVELED_INWARD);
	setBgColor(al::RGB(255,255,255));
}

void gui::TextBox::updateText()
{
	auto font = RM.get<al::Font>(txt.getFontID());
	auto sub = buffer.substr(viewPos);
	auto cut = font->calcCutoffPoint(sub, getScreenRectangle().width());
	sub = sub.substr(0, cut);

	txt.setText(al::UStr::EncodeToUTF8(sub));
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

std::string gui::TextBox::getText()
{
	return al::UStr::EncodeToUTF8(buffer);
}
void gui::TextBox::setText(const std::string_view txt)
{
	buffer = al::UStr::DecodeToUTF32(txt);
	updateText();
}

void gui::TextBox::onMouseDown(const ALLEGRO_EVENT& ev)
{
	focused = IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle());
	setBgColor(al::RGB(255-int(focused)*40,255,255));
}

void gui::TextBox::onKeyChar(const ALLEGRO_EVENT& ev)
{
	if(isFocused()) {
		if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
			deleteCharacter(buffer.size()-1);
		} else if(ev.keyboard.unichar >= 32) {
			insertCharacter(buffer.size(), ev.keyboard.unichar);
		}
	}
}

void gui::TextBox::render()
{
	al::TargetBitmap.setClippingRectangle(getScreenRectangle());
	Window::render();
	al::TargetBitmap.resetClippingRectangle();
}