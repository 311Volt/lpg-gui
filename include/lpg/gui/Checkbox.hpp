#ifndef LPG_GUI_CHECKBOX_H
#define LPG_GUI_CHECKBOX_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Image.hpp>
#include <lpg/gui/Button.hpp>

namespace gui {
	class Checkbox: public Button {
	public:
                Checkbox(float x, float y);
                Checkbox(float x, float y, float w, float h);
                virtual ~Checkbox();

                virtual std::string_view className() {return "Checkbox";};

                bool isChecked() const;

                static constexpr float DefaultWidth = 12;
                static constexpr float DefaultHeight = 12;
	private:
                virtual void updateAppearance() override;
		Image checkmark;

		bool checked;
	};
}

#endif // LPG_GUI_CHECKBOX_H
