#include <lpg/gui/TitleBar.hpp>
#include <lpg/gui/Mouse.hpp>

gui::TitleBar::TitleBar()
	: Window(48,18,1,1)
{
	setBgColor(al::Color::RGB(0,0,192));
	setEdgeType(EdgeType::NONE);

	caption.setAlignment(Alignment::LEFT_CENTER);
	caption.setTextColor(al::Color::RGB(255,255,255));
	caption.setPos({4,1});
	addChild(caption);

	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &TitleBar::onMouseDown);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_UP, &TitleBar::onMouseUp);
}

gui::TitleBar::~TitleBar()
{

}

void gui::TitleBar::onMouseDown(const ALLEGRO_EVENT& ev)
{
	if(IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle())) {
		clickPos = {ev.mouse.x, ev.mouse.y};
		startPos = ToPixels(parent->getAbsPos());
		registerEventHandler(ALLEGRO_EVENT_MOUSE_AXES, &TitleBar::onMouseMove);
	}
}

void gui::TitleBar::onMouseUp(const ALLEGRO_EVENT& ev)
{
	if(IsReleaseEvent(ev, MouseBtn::LEFT)) {
		deleteEventHandler(ALLEGRO_EVENT_MOUSE_AXES);
	}
}

void gui::TitleBar::onMouseMove(const ALLEGRO_EVENT& ev)
{
	al::Coord<int> mousePos {ev.mouse.x,ev.mouse.y};
	parent->setPos(ToUnits(mousePos + startPos - clickPos));
}

void gui::TitleBar::onTitleChange()
{
	caption.setTitle(parent->getTitle());
}

void gui::TitleBar::render()
{
	Window::render();
}

void gui::TitleBar::onAdoption()
{
	setTitle(parent->getTitle());
	resize(parent->getWidth()-3, getHeight());
}
