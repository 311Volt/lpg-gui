#ifndef INCLUDE_LPG_RESMGR_MANAGEDRESOURCE
#define INCLUDE_LPG_RESMGR_MANAGEDRESOURCE

#include <memory>
#include <type_traits>
#include <typeinfo>
#include <lpg/resmgr/ResourceLoader.hpp>
#include <axxegro/resources/Resource.hpp>
#include <axxegro/time/Time.hpp>

namespace lpg {
	class IManagedResource {
	public:
		virtual void load() = 0;
		virtual void unload() = 0;
		virtual void reload() = 0;
		virtual double timeSinceLastUse() = 0;
		virtual unsigned refCount() = 0;
		virtual const std::type_info& resourceType() = 0;
		virtual IResourceLoader* getLoader() = 0;
	};

	template<typename T>
	class ManagedResource: public IManagedResource {
		//static_assert(std::is_base_of<al::Resource, T>::value);
	public:
		using ResourceType = T;
		ManagedResource(ResourceLoader<T>* loader)
			: loader(loader),
			  timeOfLastUse(-1437.0f)
		{


		}

		virtual void load() override
		{
			if(!ptr) {
				ptr = std::shared_ptr<T>(loader->createObject());
			}
		}
		virtual void unload() override 
		{
			ptr.reset();
		}
		virtual void reload() override
		{
			unload();
			load();
		}
		virtual double timeSinceLastUse() override
		{
			return al::GetTime() - timeOfLastUse;
		}

		virtual unsigned refCount() override
		{
			return ptr.use_count();
		}

		virtual const std::type_info& resourceType() override
		{
			return typeid(T);
		}

		virtual IResourceLoader* getLoader() override
		{
			return loader.get();
		}

		std::shared_ptr<T> get()
		{
			timeOfLastUse = al::GetTime();
			load();
			return ptr;
		}
	private:
		std::unique_ptr<ResourceLoader<T>> loader;
		double timeOfLastUse;
		std::shared_ptr<T> ptr;
	};

}

#endif /* INCLUDE_LPG_RESMGR_MANAGEDRESOURCE */
