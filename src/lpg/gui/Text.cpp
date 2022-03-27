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
	try {
		std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
		
		Rect td = font->getTextDimensions(title);
		td.b.x = std::max(td.a.x+1.0f, td.b.x);
		td.b.y = std::max(td.a.x+1.0f, td.b.y);
		
		resize(td.getSize());
	} catch(std::out_of_range& e) {
		lpg::Log(1, fmt::format("out_of_range in Text::onTitleChange (does the font \"{}\" exist?)", RM.getNameOf(rID)));
	}
}

void gui::Text::render()
{
	std::shared_ptr<al::Font> font = RM.get<al::Font>(rID);
	if(title.length()) {
		Rect td = font->getTextDimensions(title);
		td.b.x = std::max(td.a.x+1.0f, td.b.x);
		td.b.y = std::max(td.a.x+1.0f, td.b.y);

		font->draw(title, textColor, -td.a);
	}
	drawChildren();
}
