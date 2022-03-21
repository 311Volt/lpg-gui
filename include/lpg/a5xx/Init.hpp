#ifndef LPG_AL_INIT_H
#define LPG_AL_INIT_H

#include <set>
#include <string>
#include <functional>

namespace al {

	struct InitTask {
		std::string name;
		std::function<bool(void)> fn;
	};

	std::set<std::string> FullInit();
}

#endif // LPG_AL_INIT_H
