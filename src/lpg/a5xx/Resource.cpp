#include <lpg/a5xx/Resource.hpp>

#include <lpg/a5xx/Log.hpp>

#include <allegro5/allegro.h>
#include <fmt/format.h>
#include <iostream>
#include <sstream>

al::Resource::Resource(const char* path, LoaderFn loaderFn, DeleteFn deleteFn)
	: Resource()
{
	sourcePath = path;
	reloadable = true;
	this->loaderFn = loaderFn;
	this->deleteFn = deleteFn;

	status = Status::UNLOADED;
}

al::Resource::Status al::Resource::getStatus()
{
	return status;
}

al::Resource::Resource()
{
	reloadable = false;
	sourcePath = "anonymous";
	status = Status::OK;
}

al::Resource::~Resource()
{
	unload();
}

void al::Resource::unload()
{
	if(alPtr.p) {
		al::Log(3, fmt::format("deleting {} @ {}", sourcePath, alPtr.p));
		deleteFn(alPtr.p);
		alPtr.p = nullptr;
	}
	status = Status::UNLOADED;
}

void al::Resource::load()
{
	if(!reloadable)
		return;
	try {
		double t0 = al_get_time();
		alPtr.p = loaderFn(sourcePath.c_str());
		if(!alPtr.p) {
			throw ResourceLoadError("loaderFn returned nullptr (likely missing or corrupted source file)");
		}
		double t1 = al_get_time();
		lastLoadingTime = t1 - t0;
		lastUsed = t1;


		Log(1, fmt::format(
			"{} loaded as {:p} in {:.3f}s",
			sourcePath,
			alPtr.p,
			lastLoadingTime
		));
		status = alPtr.p ? Status::OK : Status::ERROR;
	}
	catch(ResourceLoadError& err) {
		Log(1, fmt::format(
			"al::Resource: cannot load {}: {}",
			sourcePath,
			err.what()
		));
		setToFallback();
		status = Status::FALLBACK;
	}

}

void al::Resource::reload()
{
	if(isReloadable()) {
		unload();
		load();
	}
}

bool al::Resource::isReloadable()
{
	return reloadable;
}

void* al::Resource::get()
{
	if(status == Status::UNLOADED) {
		status = Status::LOADING;
		load();
	}
	lastUsed = al_get_time();
	return alPtr.p;
}

double al::Resource::timeSinceLastUse()
{
	return al_get_time() - lastUsed;
}


std::vector<std::string> al::Resource::parseSourceString(const std::string& str)
{
	std::vector<std::string> ret;


	auto pipe = std::find(str.begin(), str.end(), '|');
	if(pipe != str.end()) {
		std::string path(str.begin(), pipe);
		std::string args(pipe+1, str.end());

		ret.emplace_back(path);

		std::stringstream ss(args);
		for(std::string arg; ss>>arg;) {
			ret.push_back(arg);
		}
	} else {
		ret = {str};
	}

	return ret;
}
