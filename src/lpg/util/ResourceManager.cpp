#include <lpg/util/ResourceManager.hpp>

#include <fmt/format.h>

#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/resources/Font.hpp>

#include <cctype>

void lpg::ResourceManager::loadFromConfig(const al::Config& cfg)
{
	for(const auto& sectionName: cfg.sections()) {
		for(const auto& key: cfg.keys()) {
			const auto& value = cfg.getValue(sectionName, key);
			loadResource(sectionName, key, value);
		}
	}
}

void lpg::ResourceManager::registerLoader(const std::string& name, lpg::ResourceManager::ResourceLoader loader)
{
	loaders[name] = loader;
}

lpg::ResourceManager::ResourceID lpg::ResourceManager::loadResource(const std::string& type, const std::string& name, const std::string& args)
{
	if(loaders.count(name) == 0 || !loaders[name]) {
		throw ResourceLoaderNotRegistered(fmt::format(
			"cannot load \"{}\" as \"{}\": loader for {} not registered",
			args, name, type
		));
	}
	if(auto loader = loaders[name]) {
		resources.insert(std::shared_ptr<al::IResourceHandle>(loader(args)));
	}
}

lpg::ResourceManager::ResourceID lpg::ResourceManager::getIdOf(const std::string& resourceName)
{
	try {
		return nameMap.at(resourceName);
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"cannot get id of \"{}\": no such resource (std::bad_alloc: {})",
			resourceName,
			e.what()	
		));
	}
}

std::shared_ptr<al::IResourceHandle> lpg::ResourceManager::getHandle(lpg::ResourceManager::ResourceID id)
{
	try {
		resources.at(id)->load();
		return resources.at(id);
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"resource with id={} not found",
			id
		));
	}
}

std::shared_ptr<al::IResourceHandle> lpg::ResourceManager::getHandle(const std::string& name)
{
	return getHandle(getIdOf(name));
}

void lpg::ResourceManager::releaseUnusedResources()
{
	for(uint32_t i=0; i<resources.size(); i++) {
		if(!resources.contains(i)) {
			continue;
		}
		auto& r = resources[i];

		if(r->timeSinceLastUse() > defaultReleaseTime) {
			r->unload();
		}
	}
}

void lpg::ResourceManager::registerDefaultLoaders()
{
	registerLoader("Bitmap", [](const std::string& args){
		return static_cast<al::IResourceHandle*>(
			new al::BitmapHandleImgFile(args)
		);
	});

	registerLoader("Font", [](const std::string& args){
		return static_cast<al::IResourceHandle*>(
			new al::FontHandleFile(args, 10)
		);
	});
