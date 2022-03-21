#ifndef LPG_AL_RESOURCE_H
#define LPG_AL_RESOURCE_H

#include <string>
#include <functional>
#include <stdexcept>

namespace al {

	class MovableVoidPtr {
	public:
		MovableVoidPtr():p(nullptr){}
		MovableVoidPtr(void* p):p(p){}
		MovableVoidPtr(MovableVoidPtr& o) = delete;
		MovableVoidPtr(MovableVoidPtr&& o):p(o.p){o.p=nullptr;}

		void* p;
	};

	class ResourceLoadError : public std::runtime_error {
	public:
		ResourceLoadError(const std::string& msg):std::runtime_error(msg){}
		ResourceLoadError(const char* msg):std::runtime_error(msg){}
	};

	class Resource {
	public:
		using LoaderFn = std::function<void*(const char*)>;
		using DeleteFn = std::function<void(void*)>;

		Resource(const char* path, LoaderFn loaderFn, DeleteFn deleteFn);
		// copy ctor deliberately deleted by alPtr
		Resource();
		~Resource();

		virtual void setToFallback() = 0;
		virtual void unload();
		virtual void load();
		virtual void reload();

		double timeSinceLastUse();

		bool isReloadable();
		void* get();

		enum class Status: uint8_t {
			UNLOADED = 0,
			OK       = 1,
			FALLBACK = 2,
			ERROR    = 3,
			LOADING  = 4
		};

		Status getStatus();

		static std::vector<std::string> parseSourceString(const std::string& str);
	protected:

		MovableVoidPtr alPtr;
		bool reloadable;
		Status status;
		std::string sourcePath;

		double lastUsed;
		double lastLoadingTime;

		LoaderFn loaderFn;
		DeleteFn deleteFn;
	};

	template<class T>
	Resource::LoaderFn MakeLoader(std::function<T*(const char*)> fn)
	{
		return [=](const char* p)->T*{return fn(p);};
	}

	template<class T>
	Resource::DeleteFn MakeDeleter(std::function<void(T*)> fn)
	{
		return [=](void* p)->void{fn((T*)p);};
	}
}

#endif // LPG_AL_RESOURCE_H
