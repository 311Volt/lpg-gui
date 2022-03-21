#include <lpg/a5xx/Bitmap.hpp>

#include <stdexcept>
#include <iostream>

#include <allegro5/allegro_ttf.h>
#include <fmt/format.h>

#include <lpg/a5xx/RAII.hpp>
#include <lpg/a5xx/Log.hpp>

al::Bitmap::Bitmap(int w, int h)
	: Resource()
{
	reloadable = false;
	alPtr.p = al_create_bitmap(w, h);
	deleteFn = MakeDeleter<ALLEGRO_BITMAP>(al_destroy_bitmap);
}


al::Bitmap::Bitmap(const char* filename)
	: Resource(
		filename,
		MakeLoader<ALLEGRO_BITMAP>(al_load_bitmap),
		MakeDeleter<ALLEGRO_BITMAP>(al_destroy_bitmap)
	)
{

}

al::Bitmap::~Bitmap()
{

}



void al::Bitmap::setToFallback()
{
	unload();
	alPtr.p = al_create_bitmap(32, 32);
	ScopedTargetBitmap target((ALLEGRO_BITMAP*)alPtr.p);
	al_clear_to_color(al_map_rgb(0,127,127));
	ALLEGRO_FONT* f = al_create_builtin_font();
	if(!f) {
		Log(1, fmt::format(
			"fallback bitmap creation failed",
			sourcePath
		));
	}

	al_draw_text(f, al_map_rgb(255,255,255), 0, 0,  0, "miss");
	al_draw_text(f, al_map_rgb(255,255,255), 0, 8,  0, "ing");
	al_draw_text(f, al_map_rgb(255,255,255), 0, 16, 0, "text");
	al_draw_text(f, al_map_rgb(255,255,255), 0, 24, 0, "ure");

	al_destroy_font(f);
}

ALLEGRO_BITMAP* al::Bitmap::ptr()
{
	return (ALLEGRO_BITMAP*)get();
}
