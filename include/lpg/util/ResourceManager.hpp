#ifndef INCLUDE_LPG_UTIL_RESOURCEMANAGER
#define INCLUDE_LPG_UTIL_RESOURCEMANAGER

#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <string>

#include <axxegro/axxegro.hpp>
#include <lpg/util/IntegerMap.hpp>

namespace lpg {

	class ResourceNotFound: public std::runtime_error {using std::runtime_error::runtime_error;};
	class ResourceLoaderNotRegistered: public std::runtime_error {using std::runtime_error::runtime_error;};

	class ResourceManager {
	public:
		using ResourceLoader = std::function<al::IResourceHandle*(const std::string&)>;
		using ResourceID = uint32_t;

		void registerDefaultLoaders();

		void loadFromConfig(const al::Config& cfg);
		void registerLoader(const std::string& name, ResourceLoader loader);
		ResourceID loadResource(const std::string& type, const std::string& name, const std::string& args);

		ResourceID getIdOf(const std::string& resourceName);

		std::shared_ptr<al::IResourceHandle> getHandle(ResourceID id);
		std::shared_ptr<al::IResourceHandle> getHandle(const std::string& name);

		template<typename T>
		std::shared_ptr<T> get(ResourceID id)
		{
			static_assert(std::is_base_of<al::ResourceHandle<T>, T>::value);
			using THandle = al::ResourceHandle<T>;
			auto handle = getHandle(id);
			if(std::shared_ptr<THandle> tHandle = std::dynamic_pointer_cast<THandle>(handle)) {
				return tHandle->ptr();
			}
		}

		void releaseUnusedResources();
	private:
		size_t noUnloadThreshold;

		double defaultReleaseTime;

		std::unordered_map<std::string, uint32_t> nameMap;
		std::unordered_map<std::string, ResourceLoader> loaders;
		lpg::IntegerMap<uint32_t, std::shared_ptr<al::IResourceHandle>> resources;
	};
}

#endif /* INCLUDE_LPG_UTIL_RESOURCEMANAGER */
