#include <allegro5/allegro5.h>
#include <allegro5/allegro_native_dialog.h>

#include <axxegro/axxegro.hpp>

#include <lpg/gui/gui.hpp>
#include <lpg/util/Log.hpp>

#include <fstream>
#include <iostream>
#include <cmath>


#include <fmt/format.h>

int main()
{
	std::set_terminate(al::Terminate);
	al::FullInit();
	al::Bitmap::SetNewBitmapFlags(ALLEGRO_MIPMAP | ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
	lpg::Log.SetVerbosity(2);
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	
	al::Display disp(800, 600);

	al::Config resCfg("gui/default.ini");
	
	gui::Window::RM.registerDefaultLoaders();
	gui::Window::RM.loadFromConfig(resCfg);

	gui::Desktop desk;
	gui::Window win({200, 350}, {20, 20});
	win.give(std::make_unique<gui::TitleBar>());

	gui::Text close({200, 40}, {10, 30}, "Press ESC §h00FF00§r\"DefaultFont2\"to exit");
	gui::Text scaleInfo({109, 90}, {10, 50}, "Scale: 1.00");

	gui::Button upscale({70, 30}, {20, 80});
	upscale.setTitle("Scale+");
	upscale.setCallback([&](){
		gui::Window::SetEnvScale(gui::Window::GetEnvScale() * 1.2);
		scaleInfo.setText(fmt::format("Scale: {:.2f}", gui::Window::GetEnvScale()));
	});

	gui::Button downscale({70, 30}, {20, 115});
	downscale.setTitle("Scale-");
	downscale.setCallback([&](){
		gui::Window::SetEnvScale(gui::Window::GetEnvScale() / 1.2);
		scaleInfo.setText(fmt::format("Scale: {:.2f}", gui::Window::GetEnvScale()));
	});

	gui::Image stealie("SVGTest", {50, 150});
	gui::Slider slider({170, 30}, {20, 250}, 16);
	gui::Checkbox chk({10, 200});
	gui::TextBox tb({170, 20}, {10, 285});

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
	
#ifdef WIN32
	gui::Window dzi00b({290, 200}, {1, 1});
	gui::Text dziub({280, 190}, {10, 10});
	al::Config dziubCfg("dziub/default.ini");
	gui::Window::RM.loadFromConfig(dziubCfg);

	std::ifstream msgfile("dziub/message.txt");
	std::string dmsg {std::istreambuf_iterator<char>(msgfile), std::istreambuf_iterator<char>()};
	
	dzi00b.addChild(dziub);
	dziub.setText(dmsg);
	desk.addChild(dzi00b);
#endif

	desk.mainLoop();

	gui::Window::RM.releaseAllResources();

	return 0;
}
