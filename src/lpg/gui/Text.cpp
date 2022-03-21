#include <lpg/gui/Text.hpp>

#include <algorithm>

#include <lpg/a5xx/Log.hpp>

#include <allegro5/allegro_primitives.h>


gui::Text::Text(float x, float y)
	: Window(0, 0, x, y)
{
	fontName = "DefaultFont";
	setTitle("");
	enablePrerendering();
}

gui::Text::Text()
	: Text(0, 0)
{

}


gui::Text::~Text()
{

}

void gui::Text::onRescale()
{
	Window::onRescale();
	onTitleChange();
}

void gui::Text::onTitleChange()
{
	try {
		al::Font* font = RM.get<al::Font>(fontName);//getDefaultFont();

		int x,y,w,h;
		al_get_text_dimensions(font->ptr(), title.c_str(), &x, &y, &w, &h);
		w = std::max(0, w) + 1;
		h = std::max(0, h) + 1;

		Point tDims = ToUnits(Point(w,h));
		resize(tDims);
	} catch(std::out_of_range& e) {
		al::Log(1, fmt::format("out_of_range in Text::onTitleChange (does the font \"{}\" exist?)", fontName));
	}
}

void gui::Text::render()
{
	al::Font* font = RM.get<al::Font>(fontName);
	if(font && title.length()) {
		int x,y,w,h;
		al_get_text_dimensions(font->ptr(), title.c_str(), &x, &y, &w, &h);
		w = std::max(0, w) + 1;
		h = std::max(0, h) + 1;

		float sw, sh;
		getScreenSize(&sw, &sh);
		ALLEGRO_USTR* ustr = al_ustr_new_from_buffer(title.c_str(), title.size());
		al_draw_ustr(font->ptr(), textColor, -x, -y, ALLEGRO_ALIGN_INTEGER, ustr);
		//al_draw_rectangle(r1.a.x, r1.a.y, r1.b.x, r1.b.y, al_premul_rgba(0,255,0,80), 1.0);
		al_ustr_free(ustr);
	}
	drawChildren();
}
