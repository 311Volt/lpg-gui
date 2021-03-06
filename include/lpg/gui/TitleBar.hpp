#ifndef LPG_GUI_TITLEBAR_H
#define LPG_GUI_TITLEBAR_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Button.hpp>
#include <lpg/gui/Text.hpp>

#include <axxegro/resources/Font.hpp>

namespace gui {
	class TitleBar : public Window {
	public:
		TitleBar();

		LPG_WIN_CLS_NAME("TitleBar");

		virtual void render() override;
		virtual void onTitleChange() override;
		virtual void onAdoption() override;
	private:
		void onMouseDown(const ALLEGRO_EVENT& ev);
		void onMouseUp(const ALLEGRO_EVENT& ev);
		void onMouseMove(const ALLEGRO_EVENT& ev);

		al::Coord<int> clickPos, startPos;
		Text caption;
	};
}

#endif // LPG_GUI_TITLEBAR_H
