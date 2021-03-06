#include <lpg/gui/Button.hpp>
#include <lpg/gui/Mouse.hpp>

#include <lpg/util/Log.hpp>

gui::Button::Button(
	al::Vec2<> size, 
	al::Coord<> pos, 
	const std::string_view text,
	CallbackT callback
)
	: Window(size, pos, ALIGN_LEFT_TOP, EDGE_BEVELED),
	caption({1,1}, {0,0}),
	callback(callback)
{
	state = State::DEFAULT;
	trigger = {State::DOWN, State::DEFAULT};

	//caption.setAlignment(ALIGN_CENTER);
	caption.setPos({0,0});
	caption.resize(getSize());
	caption.setTextAlignment(ALIGN_CENTER);
	setTitle(std::string(text));
	
	clickPos = {0,0};
	addChild(caption);

	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &Button::onMouseDown);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_UP, &Button::onMouseUp);
	registerEventHandler(ALLEGRO_EVENT_MOUSE_AXES, &Button::onMouseMove);
}

void gui::Button::setCallback(CallbackT callback)
{
	this->callback = callback;
}

void gui::Button::setTrigger(std::pair<Button::State,Button::State> trigger)
{
	this->trigger = trigger;
}

void gui::Button::onTitleChange()
{
	caption.setText(getTitle());
}

void gui::Button::render()
{
	Window::render();
}

void gui::Button::tick()
{
	Window::tick();
}

gui::Button::State gui::Button::getState()
{
	return state;
}

void gui::Button::updateAppearance()
{
	edgeType = (state==State::DOWN) ? EDGE_BEVELED_INWARD : EDGE_BEVELED;
	caption.setPos((state==State::DOWN) ? al::Vec2<>{1.0f,1.0f} : al::Vec2<>{0.0f,0.0f});
	bgColor = ((state==State::HOVER) ? al::RGB(192,192,224) : al::RGB(192,192,192));
	needsRedraw = true;
}

void gui::Button::onMouseDown(const ALLEGRO_EVENT& ev)
{
	if(IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle())) {
		setState(State::DOWN, ev);
	}
	updateAppearance();
}

al::Coord<int> gui::Button::getLastClickMousePos()
{
	return clickPos;
}

void gui::Button::onMouseUp(const ALLEGRO_EVENT& ev)
{
	if(IsReleaseEvent(ev, MouseBtn::LEFT)) {
		if(state == State::DOWN) {
			//clickPos = Point(ev.mouse.x, ev.mouse.y);
			//callback();
		}
		setState(State::DEFAULT, ev);
	}
	updateAppearance();
}

void gui::Button::onMouseMove(const ALLEGRO_EVENT& ev)
{
	if(state == State::DEFAULT && IsMouseInRect(ev, getScreenRectangle())) {
		setState(State::HOVER, ev);
	} else if(state == State::HOVER && !IsMouseInRect(ev, getScreenRectangle())) {
		setState(State::DEFAULT, ev);
	}
}

void gui::Button::setState(gui::Button::State state, const ALLEGRO_EVENT& cause)
{
	if(std::make_pair(this->state, state) == trigger) {
		if(	cause.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN ||
			cause.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP ||
			cause.type == ALLEGRO_EVENT_MOUSE_AXES
		) {
			clickPos = {cause.mouse.x, cause.mouse.y};
			callback();
		}

	}
	this->state = state;
	switch(state) {
		case State::DEFAULT: onDefault(); break;
		case State::HOVER: onHover(); break;
		case State::DOWN: onDown(); break;
	}
}

void gui::Button::onDefault()
{
	updateAppearance();
}

void gui::Button::onHover()
{
	updateAppearance();
}

void gui::Button::onDown()
{
	updateAppearance();
}

