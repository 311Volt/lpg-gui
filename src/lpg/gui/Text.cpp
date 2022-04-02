#include <lpg/gui/Text.hpp>
#include <lpg/util/Log.hpp>

#include <algorithm>

#include <fmt/format.h>

#include <allegro5/allegro_primitives.h>


gui::Text::Text(float x, float y)
	: Window(0, 0, x, y)
{
	setFont("DefaultFont");
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

void gui::Text::setFont(const std::string& resName)
{
	rID = RM.getIdOf(resName);
}

void gui::Text::onRescale()
{
	Window::onRescale();
	onTitleChange();
}



void gui::Text::onTitleChange()
{
	std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
	
	al::Rect<int> td = font->getTextDimensions(title);
	td.b += {1, 1};
	td.b.x = std::max(td.a.x+5.0f, td.b.x+0.0f);
	td.b.y = std::max(td.a.y+5.0f, td.b.y+0.0f);

	resize(ToUnits(td.size()));
}

void gui::Text::render()
{
	std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
	if(title.length()) {
		al::Rect<int> td = font->getTextDimensions(title);
		font->draw(title, textColor, -td.a);
	}
	drawChildren();
}
