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

		Slider(int x, int y, int w, int h);
		~Slider();

		virtual std::string_view className() {return "Slider";};

		void setQuantization(uint16_t newMaxValue);

		bool isVertical();
		double getValue();
		uint16_t getRawValue();
		void setRawValue(uint16_t v);
		void setValueFunction(ValueFn valueFn);

		static constexpr int DEFAULT_HANDLE_WIDTH = 24;
	private:
		void onMouseMove(const ALLEGRO_EVENT& ev);
		Text caption;
		Point clickPos;
		ValueFn valueFn;
		Button handle;
		uint16_t maxValue;
		uint16_t value;
		int handleDragOffset;
	};
}

#endif // LPG_GUI_SLIDER_H
