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
		virtual std::type_info resourceType() = 0;
	};

	template<typename T>
	class ManagedResource {
		static_assert(std::is_base_of<al::Resource, T>::value);
	public:
		using ResourceType = T;
		ManagedResource(ResourceLoader<T> loader)
			: loader(loader),
			  timeOfLastUse(-1437.0f)
		{


		}

		virtual void load() override
		{
			ptr = std::shared_ptr<T>(loader.createObject());
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

		virtual std::type_info resourceType() override
		{
			return typeid(T);
		}

		std::shared_ptr<T> get()
		{
			timeOfLastUse = al::GetTime();
			return ptr;
		}
	private:
		double timeOfLastUse;
		ResourceLoader<T> loader;
		std::shared_ptr<T> ptr;
	};

}

#endif /* INCLUDE_LPG_RESMGR_MANAGEDRESOURCE */
