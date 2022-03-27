#ifndef INCLUDE_LPG_RESMGR_RESOURCELOADER
#define INCLUDE_LPG_RESMGR_RESOURCELOADER

#include <axxegro/resources/Resource.hpp>
#include <axxegro/math/math.hpp>

namespace lpg {

	class ScaleAwareLoader {
	public:
		virtual void setScale(al::Vec2 scale) = 0;
	};

	template<typename T>
	class ResourceLoader {
	public:
		virtual void T* createObject() = 0;
	};

	
}

#endif /* INCLUDE_LPG_RESMGR_RESOURCELOADER */
