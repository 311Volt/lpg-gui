#ifndef INCLUDE_LPG_GUI_ENVIRONMENT
#define INCLUDE_LPG_GUI_ENVIRONMENT

#include <lpg/util/ResourceManager.hpp>

namespace gui {
	class Environment {
	public:
		static void SetScale();
		static float GetScale();

		lpg::ResourceManager RM;
	private:

	};
}

#endif /* INCLUDE_LPG_GUI_ENVIRONMENT */
