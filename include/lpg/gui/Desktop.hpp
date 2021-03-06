#ifndef LPG_GUI_DESKTOP_H
#define LPG_GUI_DESKTOP_H

#include <unordered_map>
#include <string>
#include <memory>
#include <optional>

#include <axxegro/resources/Bitmap.hpp>

#include <lpg/gui/Window.hpp>
#include <lpg/resmgr/ResourceManager.hpp>

/** @file
 * The root window.
 */

namespace gui {

	class Desktop : public Window {
	public:
		Desktop();
		LPG_WIN_CLS_NAME("Desktop");

		virtual void render() override;
		void mainLoop();
		void setWallpaper(const std::string& resName);
	private:
		void onResizeEvent(const ALLEGRO_EVENT& ev);
		void onKeyDown(const ALLEGRO_EVENT& ev);

		std::optional<lpg::ResourceID> rID;
	};
}

#endif // LPG_GUI_DESKTOP_H
