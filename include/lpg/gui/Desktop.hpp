#ifndef LPG_GUI_DESKTOP_H
#define LPG_GUI_DESKTOP_H

#include <unordered_map>
#include <string>
#include <memory>

#include <axxegro/resources/Bitmap.hpp>

#include <lpg/gui/Window.hpp>

namespace gui {

	class Desktop : public Window {
	public:
		Desktop();
		~Desktop();

		virtual void render() override;
		void mainLoop();
		void setWallpaper(std::unique_ptr<al::Bitmap>&& wp);
	private:
		void onResizeEvent(const ALLEGRO_EVENT& ev);
		void onKeyDown(const ALLEGRO_EVENT& ev);

		std::unique_ptr<al::Bitmap> wallpaper;
	};
}

#endif // LPG_GUI_DESKTOP_H
