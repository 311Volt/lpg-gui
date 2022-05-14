#include <lpg/gui/Desktop.hpp>

#include <axxegro/event/EventQueue.hpp>

#include <fmt/format.h>

gui::Desktop::Desktop()
	: Window(al::CurrentDisplay.size(), {0, 0})
{
	bgColor = al::Black;
	registerEventHandler(ALLEGRO_EVENT_DISPLAY_RESIZE, &Desktop::onResizeEvent);
	registerEventHandler(ALLEGRO_EVENT_KEY_DOWN, &Desktop::onKeyDown);
}

void gui::Desktop::setWallpaper(const std::string& resName)
{
	rID = {RM.getIdOf(resName)};
}

void gui::Desktop::render()
{
	Window::render();

	if(rID.has_value()) {
		std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID.value());
		bmp->drawScaled(bmp->rect(), al::CurrentDisplay.rect());
	}

	drawChildren();
}


void gui::Desktop::mainLoop()
{
	al::EventQueue eq;
	eq.registerSource(al::GetKeyboardEventSource());
	eq.registerSource(al::GetMouseEventSource());
	eq.registerSource(al::CurrentDisplay.eventSource());
	while(!exitFlag) {
		while(!eq.empty()) {
			handleEvent(eq.pop());
		}
		tick();

		al::TargetBitmap.clearToColor(bgColor);
		draw();
		al::CurrentDisplay.flip();
	}

	printDrawTimeSummary();
}

void gui::Desktop::onResizeEvent(const ALLEGRO_EVENT& ev)
{
	static constexpr double RESIZE_FREQ = 30.0;
	static double lastResizeEvent = -30.0;

	if(ev.any.timestamp - 1.0/RESIZE_FREQ > lastResizeEvent) {
		lastResizeEvent = ev.any.timestamp;
		al::CurrentDisplay.acknowledgeResize();
	}
}

void gui::Desktop::onKeyDown(const ALLEGRO_EVENT& ev)
{
	if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		exitFlag = true;
}
