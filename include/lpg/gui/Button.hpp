#ifndef INCLUDE_LPG_GUI_BUTTON
#define INCLUDE_LPG_GUI_BUTTON

#include <axxegro/resources/Font.hpp>

#include <lpg/gui/Text.hpp>
#include <lpg/gui/Window.hpp>
#include <functional>
#include <utility>

namespace gui {
	class Button : public Window {
	public:
		using CallbackT = std::function<void(void)>;
		Button(
			al::Vec2<> size, 
			al::Coord<> pos, 
			const std::string_view text = "Button",
			CallbackT callback = [](){}
		);

		LPG_WIN_CLS_NAME("Button");

		void setCallback(CallbackT callback);
		virtual void render() override;
		virtual void tick() override;

		virtual void onTitleChange() override;

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

		al::Coord<int> getLastClickMousePos();
	protected:
		State state;
		std::pair<State,State> trigger;
		void setState(State state, const ALLEGRO_EVENT& cause);
		virtual void updateAppearance();
		Text caption;
	private:

		void onMouseDown(const ALLEGRO_EVENT& ev);
		void onMouseUp(const ALLEGRO_EVENT& ev);
		void onMouseMove(const ALLEGRO_EVENT& ev);

		al::Coord<int> clickPos;
		CallbackT callback;
	};
}

#endif /* INCLUDE_LPG_GUI_BUTTON */
