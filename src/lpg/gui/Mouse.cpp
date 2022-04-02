#include <lpg/gui/Mouse.hpp>

#include <iostream>
#include <fmt/format.h>

bool gui::IsMouseInRect(const ALLEGRO_EVENT& ev, const al::Rect<int>& r)
{
	return (
		ev.mouse.x >= r.a.x &&
		ev.mouse.y >= r.a.y &&
		ev.mouse.x <= r.b.x &&
		ev.mouse.y <= r.b.y
	);
}

bool gui::IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn)
{
	return (
		ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN &&
		ev.mouse.button == static_cast<unsigned>(btn)
	);
}

bool gui::IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn)
{
	return (
		ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP &&
		ev.mouse.button == static_cast<unsigned>(btn)
	);
}

bool gui::IsClickEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const al::Rect<int>& r)
{
	return IsClickEvent(ev, btn) && IsMouseInRect(ev, r);
}

bool gui::IsReleaseEvent(const ALLEGRO_EVENT& ev, MouseBtn btn, const al::Rect<int>& r)
{
	return IsReleaseEvent(ev, btn) && IsMouseInRect(ev, r);
}

