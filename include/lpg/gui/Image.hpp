#ifndef LPG_GUI_IMAGE_H
#define LPG_GUI_IMAGE_H

#include <lpg/gui/Window.hpp>

#include <axxegro/resources/Bitmap.hpp>

namespace gui {
	class Image: public Window {
	public:
		Image(const std::string& resName, float x, float y);
		void setTo(const std::string& resName);
		virtual void render() override;
		virtual void onRescale() override;

		bool visible;
	private:
		std::string resName;
		al::Bitmap* bmp;
	};
}

#endif // LPG_GUI_IMAGE_H
