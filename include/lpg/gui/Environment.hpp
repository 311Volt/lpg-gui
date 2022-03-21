#ifndef LPG_GUI_ENVIRONMENT_H
#define LPG_GUI_ENVIRONMENT_H

#include "../util/ResourceManager.hpp"

namespace gui {
	class Environment {
	public:
		static void SetScale();
		static float GetScale();

		lpg::ResourceManager RM;
	private:

	};
}

#endif // LPG_GUI_ENVIRONMENT_H
