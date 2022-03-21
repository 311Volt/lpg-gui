#ifndef LPG_GUI_BUTTON_H
#define LPG_GUI_BUTTON_H

#include "../a5xx/Font.hpp"
#include "Text.hpp"
#include "Window.hpp"
#include <functional>
#include <utility>

namespace gui {
	class Button : public Window {
	public:
		using CallbackT = std::function<void(void)>;
		Button(float w, float h, float x, float y, CallbackT callback);
		Button(float w, float h, float x, float y);

		void setCallback(CallbackT callback);
		virtual void render() override;
		virtual void tick() override;

		virtual void onDefault();
		virtual void onHover();
		virtual void onDown();
		enum class State: uint8_t {
			DEFAULT = 0,
			HOVER = 1,
			DOWN = 2
		};
		State getState();
		void setTrigger(std::pair<State,State> trigger);

		Point getLastClickMousePos();
	protected:
		State state;
		std::pair<State,State> trigger;
		void setState(State state, const ALLEGRO_EVENT& cause);
	private:
		virtual void updateAppearance();

		void onMouseDown(const ALLEGRO_EVENT& ev);
		void onMouseUp(const ALLEGRO_EVENT& ev);
		void onMouseMove(const ALLEGRO_EVENT& ev);

		Point clickPos;
		al::Font font;
		Text caption;
		CallbackT callback;
	};
}

#endif // LPG_GUI_BUTTON_H
