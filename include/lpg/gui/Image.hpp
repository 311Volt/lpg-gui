#ifndef LPG_GUI_IMAGE_H
#define LPG_GUI_IMAGE_H

#include <lpg/gui/Window.hpp>

#include <axxegro/resources/Bitmap.hpp>

#include <lpg/resmgr/ResourceManager.hpp>

namespace gui {
	class Image: public Window {
	public:
		Image(const std::string& resName, float x, float y);

		virtual std::string_view className() {return "Image";};

		void setTo(const std::string& resName);
		virtual void render() override;

	private:
		std::string resName;
		lpg::ResourceID rID;
	};
}

#endif // LPG_GUI_IMAGE_H
