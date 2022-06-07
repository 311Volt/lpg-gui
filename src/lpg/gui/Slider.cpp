#include <lpg/gui/Slider.hpp>
#include <lpg/util/Log.hpp>

#include <algorithm>
#include <utility>

void gui::SliderButton::onDefault()
{

}
void gui::SliderButton::onHover()
{

}
void gui::SliderButton::onDown()
{

}

gui::Slider::Slider(al::Vec2<> size, al::Vec2<> pos, uint16_t quant)
	: Window(size, pos, ALIGN_LEFT_TOP, EDGE_BEVELED_INWARD),
	valueFn([](uint16_t v){return v;}),
	handle(size, {0,0}),
	maxValue(65535),
	handleDragOffset(0)
{
	caption.resize(getSize());
	caption.setTextAlignment(ALIGN_CENTER);
	setQuantization(quant);
	setBgColor(al::RGB(128,128,128));
	setEdgeType(EDGE_BEVELED_INWARD);
	setRawValue(0);

	if(isVertical()) {
		handle.resize(handle.getWidth(), DEFAULT_HANDLE_WIDTH);
	} else {
		handle.resize(DEFAULT_HANDLE_WIDTH, handle.getHeight());
	}

        handle.setCallback([this](){
		al::Coord<> cl = handle.getLastClickMousePos();
		al::Coord<> sc = handle.getScreenRectangle().a;
		handleDragOffset = isVertical() ? (cl.y-sc.y) : (cl.x-sc.x);
		registerEventHandler(ALLEGRO_EVENT_MOUSE_AXES, &Slider::onMouseMove);
	});
	handle.setTrigger(std::make_pair(Button::State::HOVER, Button::State::DOWN));
	handle.setTitle("");
	caption.setAlignment(ALIGN_CENTER);
	addChild(handle);
	addChild(caption);
}


void gui::Slider::onMouseMove(const ALLEGRO_EVENT& ev)
{
	if(maxValue && handle.getState() == Button::State::DOWN) {
		al::Vec2<> hSize = handle.getScreenRectangle().size();
		al::Rect<> sc = getScreenRectangle();
		int hw = isVertical() ? hSize.y : hSize.x;
                float m0 = (isVertical()?sc.a.y:sc.a.x) + handleDragOffset;
                float m1 = (isVertical()?sc.b.y:sc.b.x) - hw + handleDragOffset;
                float mp = std::clamp(float(isVertical()?ev.mouse.y:ev.mouse.x), m0, m1);

                float f = (mp-m0) / (m1-m0);
                setRawValue(f*maxValue);

		al::Rect<> r = getRect(), hr = handle.getRect();
		float s0 = isVertical() ? r.a.y : r.a.x;
		float s1 = isVertical() ? (r.b.y-hr.size().y) : (r.b.x-hr.size().x);
		float fp = ((float)value/(float)maxValue);
		float sp = fp*(s1-s0);
		if(isVertical()) {
			handle.setPos(al::Coord<>(handle.getRelX(), sp));
		} else {
			handle.setPos(al::Coord<>(sp, handle.getRelY()));
		}

	} else {
		deleteEventHandler(ALLEGRO_EVENT_MOUSE_AXES);
	}
}

void gui::Slider::setQuantization(uint16_t newMaxValue)
{
	if(value > newMaxValue) {
		setRawValue(newMaxValue);
	}
	maxValue = newMaxValue;
}

bool gui::Slider::isVertical()
{
	return getHeight()>getWidth();
}

double gui::Slider::getValue()
{
	return valueFn(value);
}

uint16_t gui::Slider::getRawValue()
{
	return value;
}

void gui::Slider::setRawValue(uint16_t v)
{
	if(v > maxValue) {
		lpg::Log(2, fmt::format(
			"slider #{}: setRawValue: given value {} is larger than maxValue {}, ignoring",
			getID(), v, maxValue
		));
		return;
	}
	value = v;
	auto cText = fmt::format("{}",value);
	caption.setText(cText);
}

void gui::Slider::setValueFunction(ValueFn valueFn)
{
	this->valueFn = valueFn;
}
