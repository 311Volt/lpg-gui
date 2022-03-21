#include <lpg/a5xx/Init.hpp>

#include <cstdlib>
#include <vector>
#include <exception>

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>

std::set<std::string> al::FullInit()
{
	std::set<std::string> ret;
	if(!al_init()) {
		al_show_native_message_box(NULL, "error", "Allegro initialization failed", "", NULL, 0);
		std::terminate();
	}

#define INITTASK(x) {#x, x}
	std::vector<InitTask> initTasks{
		INITTASK(al_install_mouse),
		INITTASK(al_install_keyboard),
		INITTASK(al_install_audio),
		INITTASK(al_init_acodec_addon),
		INITTASK(al_init_image_addon),
		INITTASK(al_init_font_addon),
		INITTASK(al_init_ttf_addon),
		INITTASK(al_init_native_dialog_addon),
		INITTASK(al_init_primitives_addon),
	};
#undef INITTASK

	// TODO: distinguish between mandatory and optional tasks
	for(auto& task: initTasks) {
		if(!task.fn()) {
			std::string msg(task.name);
			msg += "() failed";
			al_show_native_message_box(NULL, "error", msg.c_str(), "", NULL, 0);
			continue;
		}
		ret.insert(task.name);
	}

	al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

	return ret;
}
