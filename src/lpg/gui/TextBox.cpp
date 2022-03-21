#include <lpg/gui/TextBox.hpp>

#include <algorithm>

gui::TextBox::TextBox(int x, int y, int w, int h)
	: Window(w, h, x, y)
{
	txt.setTitle("Sample Text");
	txt.setAlignment(Alignment::LEFT_CENTER);
	txt.setPos(Point(4,0));
	addChild(txt);
	addChild(cursor);
	
	cursor.setTitle("|");
	
	registerEventHandler(ALLEGRO_EVENT_KEY_CHAR, &TextBox::onKeyChar);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &TextBox::onMouseDown);
	
	setEdgeType(EdgeType::BEVELED_INWARD);
	setBgColor(al_map_rgb(255,255,255));
}


void gui::TextBox::insertCharacter(int position, int32_t codepoint)
{
	if(position < 0 || position > txt.getTitle().size()) {
		return;
	}
	char u8buf[8];
	size_t charSize = al_utf8_encode(u8buf, codepoint);
	u8buf[charSize] = 0;
	std::string str = txt.getTitle();
	
	ALLEGRO_USTR* ustr = al_ustr_new_from_buffer(txt.getTitle().c_str(), txt.getTitle().size());
	
	auto pivot = str.begin() + al_ustr_offset(ustr, position);
	
	std::string result = 
		std::string(str.begin(), pivot) + 
		std::string(u8buf) + 
		std::string(pivot, str.end())
	;
	txt.setTitle(result);
	
	al_ustr_free(ustr);
}

void gui::TextBox::deleteCharacter(int position)
{
	if(position < 0 || position > txt.getTitle().size()) {
		return;
	}
	
	ALLEGRO_USTR* ustr = al_ustr_new_from_buffer(txt.getTitle().c_str(), txt.getTitle().size());
	
	//auto off0 = str.begin() + al_ustr_offset(ustr, )
	
}

void gui::TextBox::onMouseDown(const ALLEGRO_EVENT& ev)
{
	focused = IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle());
	setBgColor(al_map_rgb(255-int(focused)*40,255,255));
}


void gui::TextBox::onKeyChar(const ALLEGRO_EVENT& ev)
{
	char u8buf[8];
	if(isFocused()) {
		if(ev.keyboard.keycode == ALLEGRO_KEY_BACKSPACE) {
			//deleteCharacter()
		} else if(ev.keyboard.unichar) {
			size_t s = al_utf8_encode(u8buf, ev.keyboard.unichar);
			u8buf[s] = 0;
			txt.setTitle(txt.getTitle() + std::string(u8buf));
		}
	}
}
