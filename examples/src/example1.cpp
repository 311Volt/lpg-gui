#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <lpg/gui/gui.hpp>
#include <lpg/util/Log.hpp>

#include <iostream>
#include <cmath>

#include <fmt/format.h>

int main1()
{
	al::FullInit();
	lpg::Log.SetVerbosity(2);
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	al_create_display(1024, 768);

	gui::Window::RM.loadFromConfig(al::Config("gui/default.ini"));

	gui::Desktop desk;
	gui::Window win(200, 350, 20, 20);
	win.give(std::make_unique<gui::TitleBar>());

	gui::Text close(10, 30);
	close.setTitle("Press ESC to exit");

	gui::Text scaleInfo(10, 50);
	scaleInfo.setTitle("Scale: 1.00");

	gui::Button upscale(70, 30, 20, 80);
	upscale.setTitle("Scale+");
	upscale.setCallback([&](){
		gui::Window::SetEnvScale(gui::Window::GetEnvScale() * 1.2);
		scaleInfo.setTitle(fmt::format("Scale: {:.2f}", gui::Window::GetEnvScale()));
	});

	gui::Button downscale(70, 30, 20, 115);
	downscale.setTitle("Scale-");
	downscale.setCallback([&](){
		gui::Window::SetEnvScale(gui::Window::GetEnvScale() / 1.2);
		scaleInfo.setTitle(fmt::format("Scale: {:.2f}", gui::Window::GetEnvScale()));
	});

	gui::Image stealie("SVGTest", 50, 150);

	gui::Slider slider(20, 250, 170, 30);
	slider.setQuantization(16);

	gui::Checkbox chk(10, 200);
	
	gui::TextBox tb(10, 285, 170, 20);

	desk.addChild(win);
	desk.setWallpaper("Wallpaper");

	win.addChild(chk);
	win.addChild(scaleInfo);
	win.addChild(upscale);
	win.addChild(downscale);
	win.addChild(close);
	win.addChild(stealie);
	win.addChild(slider);
	win.addChild(tb);

	desk.mainLoop();

	gui::Window::RM.releaseAllResources();

	return 0;
}

int main()
{
	try {
		return main1();
	} catch(std::exception& e) {
		al_show_native_message_box(NULL, "Error", "Unhandled exception: ", e.what(), "", ALLEGRO_MESSAGEBOX_ERROR);
	}
}
