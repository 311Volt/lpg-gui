#ifndef LPG_GUI_SLIDER_H
#define LPG_GUI_SLIDER_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Button.hpp>
#include <lpg/gui/Text.hpp>

#include <cstdint>
#include <functional>

namespace gui {
	class SliderButton: public Button {
	public:
		virtual void onDefault() override;
		virtual void onHover() override;
		virtual void onDown() override;
	};

	class Slider: public Window {
	public:
		using ValueFn = std::function<double(uint16_t)>;

		Slider(al::Vec2<> size, al::Vec2<> pos, uint16_t quant = 16);
		LPG_WIN_CLS_NAME("Slider");

		void setQuantization(uint16_t newMaxValue);

		bool isVertical();
		double getValue();
		uint16_t getRawValue();
		void setRawValue(uint16_t v);
		void setValueFunction(ValueFn valueFn);
		bool isBeingDragged() const;

		static constexpr int DEFAULT_HANDLE_WIDTH = 24;
		void onScroll(const ALLEGRO_EVENT& ev);
	private:
		void onMouseMove(const ALLEGRO_EVENT& ev);
		void onMouseAxesIdle(const ALLEGRO_EVENT& ev);
		Text caption;
		ValueFn valueFn;
		Button handle;
		uint16_t maxValue;
		uint16_t value;
		int handleDragOffset;
		bool beingDragged;
	};
}

#endif // LPG_GUI_SLIDER_H
