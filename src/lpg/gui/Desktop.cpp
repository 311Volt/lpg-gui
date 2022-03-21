#include <lpg/gui/Desktop.hpp>

#include <lpg/a5xx/EventQueue.hpp>

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

void gui::Desktop::render()
{
	if(wallpaper.get() && wallpaper->getStatus() != al::Resource::Status::ERROR) {
		int w = al_get_bitmap_width(wallpaper->ptr());
		int h = al_get_bitmap_height(wallpaper->ptr());
		int dw = al_get_display_width(al_get_current_display());
		int dh = al_get_display_height(al_get_current_display());
		al_draw_scaled_bitmap(wallpaper->ptr(), 0, 0, w, h, 0, 0, dw, dh, 0);
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

void gui::Desktop::setWallpaper(std::unique_ptr<al::Bitmap>&& wp)
{
	this->wallpaper = std::move(wp);
}
