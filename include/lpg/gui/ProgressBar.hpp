#ifndef LPG_GUI_INCLUDE_LPG_GUI_PROGRESSBAR
#define LPG_GUI_INCLUDE_LPG_GUI_PROGRESSBAR

#include <lpg/gui/gui.hpp>

namespace gui {

	class ProgressBar: public gui::Window {
		Window fill;
	public:
		ProgressBar(al::Vec2<> size, al::Coord<> pos);

		void setValue(float value);
	};
}


#endif /* LPG_GUI_INCLUDE_LPG_GUI_PROGRESSBAR */
