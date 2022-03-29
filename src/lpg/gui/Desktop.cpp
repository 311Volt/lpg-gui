#include <lpg/gui/Desktop.hpp>

#include <axxegro/event/EventQueue.hpp>

#include <fmt/format.h>

gui::Desktop::Desktop()
	: Window(
		al_get_display_width(al_get_current_display()),
		al_get_display_height(al_get_current_display())
	)
{
	registerEventHandler(ALLEGRO_EVENT_DISPLAY_RESIZE, &Desktop::onResizeEvent);
	registerEventHandler(ALLEGRO_EVENT_KEY_DOWN, &Desktop::onKeyDown);
}

gui::Desktop::~Desktop()
{

}

void gui::Desktop::setWallpaper(const std::string& resName)
{
	rID = {RM.getIdOf(resName)};
}

void gui::Desktop::render()
{
	al::Vec2 dispSize {
		al_get_display_width(al_get_current_display()),
		al_get_display_height(al_get_current_display())
	};
	if(rID.has_value()) {
		std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID.value());
		bmp->drawScaled(bmp->getRect(), {{0,0}, dispSize});
	}

	drawChildren();
}


void gui::Desktop::mainLoop()
{
	al::EventQueue eq;
	eq.registerSource(al_get_keyboard_event_source());
	eq.registerSource(al_get_mouse_event_source());
	eq.registerSource(al_get_display_event_source(al_get_current_display()));
	while(!exitFlag) {

		while(!eq.empty()) {
			ALLEGRO_EVENT ev = eq.pop();
			handleEvent(ev);
		}
		tick();

		al_clear_to_color(al_map_rgb(0,0,0));
		draw();
		al_flip_display();
	}

	printDrawTimeSummary();
}

void gui::Desktop::onResizeEvent(const ALLEGRO_EVENT& ev)
{
	static constexpr double RESIZE_FREQ = 30.0;
	static double lastResizeEvent = -30.0;

	if(ev.any.timestamp - 1.0/RESIZE_FREQ > lastResizeEvent) {
		lastResizeEvent = ev.any.timestamp;
		al_acknowledge_resize(al_get_current_display());
	}
}

void gui::Desktop::onKeyDown(const ALLEGRO_EVENT& ev)
{
	if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
		exitFlag = true;
}
