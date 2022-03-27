#include <lpg/resmgr/ResourceManager.hpp>

#include <lpg/util/StrManip.hpp>

#include <fmt/format.h>

#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/resources/Font.hpp>

#include <cctype>

#include <lpg/resmgr/DefaultLoaders.hpp>

void lpg::ResourceManager::loadFromConfig(const al::Config& cfg)
{
	for(const auto& sectionName: cfg.sections()) {
		for(const auto& key: cfg.keys()) {
			const auto& value = cfg.getValue(sectionName, key);
			addResource(sectionName, key, value);
		}
	}
}
void lpg::ResourceManager::registerLoader(const std::string& typeName, lpg::ResourceManager::TypeLoader loader)
{
	loaders[typeName] = loader;
}
lpg::ResourceManager::ResourceID lpg::ResourceManager::addResource(const std::string& type, const std::string& name, const std::string& args)
{
	if(loaders.count(name) == 0 || !loaders[name]) {
		throw ResourceLoaderNotRegistered(fmt::format(
			"cannot load \"{}\" as \"{}\": loader for {} not registered",
			args, name, type
		));
	}
	auto loader = loaders[name];

	resources.insert(std::unique_ptr<IManagedResource>(loader(args)));
	auto id = resources.getLowestFreeKey();
	nameMap[name] = id;
	idMap[id] = name;
}

lpg::ResourceManager::ResourceID lpg::ResourceManager::getIdOf(const std::string& resourceName)
{
	try {
		return nameMap.at(resourceName);
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"resource with name={} not found",
			resourceName
		));
	}
}

void lpg::ResourceManager::checkUnique(lpg::ResourceManager::ResourceID id)
{
	auto& res = resources[id];
	if(res->refCount() > 1) {
		throw ResourcePtrNotUnique(fmt::format(
			"ResourceManager: stray reference to a resource (id={}, name=\"{}\") detected. "
			"Do not store shared_ptrs to resources in between frames",
			id, 
			idMap[id]
		));
	}
}

void lpg::ResourceManager::releaseUnusedResources()
{
	for(uint32_t i=0; i<resources.size(); i++) {
		if(!resources.contains(i)) {
			continue;
		}
		checkUnique(i);
		auto& r = resources[i];
		
		if(r->timeSinceLastUse() > defaultReleaseTime) {
			r->unload();
		}
	}
}

void lpg::ResourceManager::releaseAllResources()
{
	for(uint32_t i=0; i<resources.size(); i++) {
		if(!resources.contains(i)) {
			continue;
		}
		checkUnique(i);
		resources[i]->unload();
	}
}


void lpg::ResourceManager::throwMismatch(std::type_info expected, std::type_info actual, lpg::ResourceManager::ResourceID id)
{
	throw ResourceTypeMismatch(fmt::format(
		"resource type mismatch: get<{}> called on \"{}\" (id={}), which holds a resource of type {}",
		expected.name(),
		idMap[id],
		id,
		actual.name()
	));
}

IManagedResource* getHandle(ResourceID id)
{
	try {
		return resources.at(id);
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"resource with id={} not found",
			id
		));
	}
}

IManagedResource* DefaultBitmapLoader(const std::string& args)
{
	return static_cast<IManagedResource*>(new lpg::ManagedResource<al::Bitmap>(lpg::ImageFileLoader(args)));
}

IManagedResource* DefaultFontLoader(const std::string& args)
{
	auto vArgs = lpg::str::Split(args, "|");
	for(auto& s: vArgs)
		s = lpg::str::Strip(s);
	
	auto filename = vArgs.at(0);
	double size = 16;
	if(vArgs.size() > 1) {
		size = std::stod(vArgs.at(1));
	}
	return static_cast<IManagedResource*>(new lpg::ManagedResource<al::Font>(lpg::FontFileLoader(filename, size)));
}



void lpg::ResourceManager::registerDefaultLoaders()
{
	registerLoader("Bitmap", DefaultBitmapLoader);
	registerLoader("Font", DefaultFont);
	
}
