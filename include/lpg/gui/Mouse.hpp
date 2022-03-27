#ifndef LPG_GUI_MOUSE_H
#define LPG_GUI_MOUSE_H

#include <allegro5/allegro.h>

#include <axxegro/math/math.hpp>

namespace gui {
	enum class MouseBtn: unsigned {
		LEFT = 1,
		MIDDLE = 3,
		RIGHT = 2
	};

	bool IsMouseInRect(const ALLEGRO_EVENT& ev, const al::Rect& r);

	bool IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn);
	bool IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn);

	bool IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const al::Rect& r);
	bool IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const al::Rect& r);
}

#endif // LPG_GUI_MOUSE_H
