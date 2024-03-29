#include <lpg/gui/Desktop.hpp>

#include <axxegro/event/EventQueue.hpp>

#include <format>

gui::Desktop::Desktop()
	: Window(al::CurrentDisplay.size(), {0, 0})
{
	bgColor = al::Black;
	registerEventHandler(ALLEGRO_EVENT_DISPLAY_RESIZE, &Desktop::onResizeEvent);
	registerEventHandler(ALLEGRO_EVENT_KEY_DOWN, &Desktop::onKeyDown);

	eventDispatcher.setEventTypeHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, [this](const ALLEGRO_EVENT& ev){
		al::Coord<> mousePos(ev.mouse.x, ev.mouse.y);
		std::vector<uint32_t> candidates;
		for(auto& childId: children) {
			Window* child = GetWindowByID(childId);
			if(child && child->getScreenRectangle().contains(mousePos)) {
				candidates.push_back(childId);
			}
		}

		//TODO this is copied from Window.cpp (drawChildren) and inefficient (not that it matters here)
		std::sort(candidates.begin(), candidates.end(), [](uint32_t l, uint32_t r){
			int zl = GetWindowByID(l)->getZIndex();
			int zr = GetWindowByID(r)->getZIndex();
			if(zl != zr)
				return zl > zr;
			return l > r;
		});

		if(candidates.size()) {
			Window* win = GetWindowByID(candidates.back());
			win->handleEvent(ev);
			win->setZIndex(9999999);
		}
		normalizeChildrenZIndices();
	});
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

	//printDrawTimeSummary();
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
