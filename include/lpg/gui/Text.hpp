#ifndef LPG_GUI_TEXT_H
#define LPG_GUI_TEXT_H

#include <lpg/gui/Window.hpp>

#include <axxegro/math/math.hpp>
#include <axxegro/resources/Font.hpp>
#include <axxegro/resources/Bitmap.hpp>

#include <lpg/resmgr/ResourceManager.hpp>

namespace gui {
	class Text : public Window {
	public:
		Text();
		Text(float x, float y);
		~Text();

		virtual std::string_view className() {return "Text";};

		void setFont(const std::string& resName);

		virtual void onRescale() override;
		virtual void onTitleChange() override;
		virtual void render() override;

		lpg::ResourceManager::ResourceID getFontID();
	private:
		lpg::ResourceManager::ResourceID rID;
		std::string text;
	};
}

#endif // LPG_GUI_TEXT_H
