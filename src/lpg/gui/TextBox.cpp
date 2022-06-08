#include <lpg/gui/TextBox.hpp>

#include <algorithm>
#include <axxegro/UStr.hpp>

gui::TextBox::TextBox(al::Vec2<> size, al::Vec2<> pos, const std::string& initialText)
	: Window(size, pos)
{
	txt.resize(this->dims);
	cursor.resize(this->dims);

	txt.resize(getSize());
	txt.setPos({0,0});
	addChild(txt);
	addChild(cursor);

	viewPos = 0;
	cursorPos = 0;

	txt.setTextAlignment(ALIGN_LEFT_CENTER);
	txt.setPadding({4,2,2,2});
	buffer = al::ToUTF32(initialText);
	updateText();
	
	cursor.setText("|");
	cursor.visible = false;
	cursor.setPadding({0,0,0,0});
	cursor.setTextAlignment(ALIGN_LEFT_CENTER);
	
	registerEventHandler(ALLEGRO_EVENT_KEY_CHAR, &TextBox::onKeyChar);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &TextBox::onMouseDown);
	
	setEdgeType(EDGE_BEVELED_INWARD);
	setBgColor(al::RGB(255,255,255));

	focused = false;
}

void gui::TextBox::updateText()
{
	/*
	auto font = RM.get<al::Font>(txt.getFontID());
	auto sub = buffer.substr(viewPos);
	auto cut = font->calcCutoffPoint(sub, getScreenRectangle().width());
	sub = sub.substr(0, cut);
	*/


	txt.setText(al::ToUTF8(buffer));
	txt.updateIfNeeded();
	cursor.setPos(al::Coord<>(txt.getSpan().b.x, 0));
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
	return al::ToUTF8(buffer);
}
void gui::TextBox::setText(const std::string_view txt)
{
	buffer = al::ToUTF32(txt);
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

void gui::TextBox::tick()
{
	static constexpr double CURSOR_BLINK_FREQ = 2.0;
	double x = fmod(al::GetTime() * CURSOR_BLINK_FREQ, 1.0);

	cursor.visible = isFocused() && x<0.5;

	Window::tick();
}

void gui::TextBox::render()
{
	al::TargetBitmap.setClippingRectangle(getScreenRectangle());
	Window::render();
	al::TargetBitmap.resetClippingRectangle();
}