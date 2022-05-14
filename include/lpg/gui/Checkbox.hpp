#ifndef LPG_GUI_CHECKBOX_H
#define LPG_GUI_CHECKBOX_H

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Image.hpp>
#include <lpg/gui/Button.hpp>

namespace gui {
	class Checkbox: public Button {
	public:
		Checkbox(al::Vec2<> size, al::Coord<> pos);
		Checkbox(al::Coord<> pos);
		LPG_WIN_CLS_NAME("Checkbox");

		bool isChecked() const;

		static constexpr al::Vec2<> DefaultSize = {12, 12};
	private:
		virtual void updateAppearance() override;
		Image checkmark;

		bool checked;
	};
}

#endif // LPG_GUI_CHECKBOX_H
