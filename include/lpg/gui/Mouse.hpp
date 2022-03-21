#ifndef LPG_GUI_MOUSE_H
#define LPG_GUI_MOUSE_H

#include "Math.hpp"
#include <allegro5/allegro.h>

namespace gui {
	enum class MouseBtn: unsigned {
		LEFT = 1,
		MIDDLE = 3,
		RIGHT = 2
	};

	bool IsMouseInRect(const ALLEGRO_EVENT& ev, const Rect& r);

	bool IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn);
	bool IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn);

	bool IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const Rect& r);
	bool IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const Rect& r);
}

#endif // LPG_GUI_MOUSE_H
