#ifndef INCLUDE_LPG_UTIL_RESOURCEMANAGER
#define INCLUDE_LPG_UTIL_RESOURCEMANAGER

#include <stdexcept>
#include <unordered_map>
#include <functional>
#include <string>
#include <typeinfo>

#include <axxegro/axxegro.hpp>

#include <lpg/resmgr/ManagedResource.hpp>
#include <lpg/util/IntegerMap.hpp>

namespace lpg {

	class ResourceNotFound: public std::runtime_error {using std::runtime_error::runtime_error;};
	class ResourceLoaderNotRegistered: public std::runtime_error {using std::runtime_error::runtime_error;};
	class ResourcePtrNotUnique: public std::runtime_error {using std::runtime_error::runtime_error;};
	class ResourceTypeMismatch: public std::runtime_error {using std::runtime_error::runtime_error;};

	class ResourceManager {
	public:
		using TypeLoader = std::function<IManagedResource*(const std::string&)>;
		using ResourceID = uint32_t;

		void registerDefaultLoaders();

		void loadFromConfig(const al::Config& cfg);
		void registerLoader(const std::string& typeName, TypeLoader loader);
		ResourceID addResource(const std::string& type, const std::string& name, const std::string& args);

		ResourceID getIdOf(const std::string& resourceName);
		std::string getNameOf(ResourceID id);

		template<typename T>
		std::shared_ptr<T> get(ResourceID id)
		{
			//static_assert(std::is_base_of<al::Resource, T>::value);
			IManagedResource* mr = getHandle(id);
			if(ManagedResource<T>* mrt = dynamic_cast<ManagedResource<T>*>(mr)) {
				return mrt->get();
			}
			throwMismatch(typeid(T), mr->resourceType(), id);
			return {nullptr}; //to suppress compiler warnings; line above always throws anyway
		}

		template<typename T>
		std::shared_ptr<T> get(const std::string& name)
		{
			return get<T>(getIdOf(name));
		}

		void setScale(al::Vec2 scale);

		void releaseUnusedResources();
		void releaseAllResources();
	private:
		void checkUnique(ResourceID id);
		void throwMismatch(const std::type_info& expected, const std::type_info& actual, ResourceID id);
		IManagedResource* getHandle(ResourceID id);

		size_t noUnloadThreshold;

		double defaultReleaseTime;

		std::unordered_map<std::string, uint32_t> nameMap;
		std::unordered_map<uint32_t, std::string> idMap;
		std::unordered_map<std::string, TypeLoader> loaders;
		lpg::IntegerMap<uint32_t, std::unique_ptr<IManagedResource>> resources;
	};
}

#endif /* INCLUDE_LPG_UTIL_RESOURCEMANAGER */
