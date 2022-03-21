#ifndef LPG_GUI_TITLEBAR_H
#define LPG_GUI_TITLEBAR_H

#include "Window.hpp"
#include "Button.hpp"

#include "../a5xx/Font.hpp"
#include "Text.hpp"

namespace gui {
	class TitleBar : public Window {
	public:
		TitleBar();
		~TitleBar();

		virtual void render() override;
		virtual void onTitleChange() override;
		virtual void onAdoption() override;
	private:
		void onMouseDown(const ALLEGRO_EVENT& ev);
		void onMouseUp(const ALLEGRO_EVENT& ev);
		void onMouseMove(const ALLEGRO_EVENT& ev);

		Point clickPos, startPos;
		Text caption;
	};
}

#endif // LPG_GUI_TITLEBAR_H
