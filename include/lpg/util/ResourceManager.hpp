#ifndef LPG_RESMGR_H
#define LPG_RESMGR_H

#include <axxegro/resources/Resource.hpp>
#include <axxegro/resources/Config.hpp>

#include <unordered_map>
#include <memory>
#include <fmt/format.h>

#include <lpg/util/IntegerMap.hpp>

namespace lpg {
	class ResourceManager {
	public:
		ResourceManager();
		~ResourceManager();

		bool load(const std::string& path);
		bool loadFromConfig(const std::string& path);

		uint32_t give(std::unique_ptr<al::Resource> res, const std::string& name);

		void releaseUnusedResources();


		template<class T>
		T* get(const std::string& name)
		{
			T* ret = dynamic_cast<T*>(getPtr(name));
			if(!ret) {
				throw std::runtime_error(fmt::format(
					"{}: resource type mismatch",
					name
				));
			}
			return ret;
		}

		/* hack alert: for global objects, this must be called at the end of main()
		 * before Allegro5 starts deinitializing everything, otherwise
		 * the dtors end up calling al_destroy_X when it is no longer valid
		 * resulting in a segfault
		 *
		 * TODO: don't use global ResourceManager objects? */
		void clear();

		/* TODO make this shit not public */
		IntegerMap<uint32_t, std::unique_ptr<al::Resource>> idMap;
		std::unordered_map<std::string, uint32_t> nameMap;
	private:
		double releaseTime;

		al::Resource* getPtr(const std::string& name);
	};
};


#endif // LPG_RESMGR_H
