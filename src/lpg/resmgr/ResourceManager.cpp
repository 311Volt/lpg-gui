#include <lpg/resmgr/ResourceManager.hpp>
#include <lpg/resmgr/DefaultLoaders.hpp>

#include <lpg/util/StrManip.hpp>

#include <fmt/format.h>

#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/resources/Font.hpp>

#include <lpg/resmgr/SVGLoader.hpp>

#include <lpg/util/Log.hpp>

#include <cctype>


void lpg::ResourceManager::loadFromConfig(const al::Config& cfg)
{
	lpg::Log(3, "initializing ResourceManager from config");
	for(const auto& sectionName: cfg.sections()) {
		lpg::Log(3, fmt::format("section [{}]", sectionName));
		for(const auto& key: cfg.keys(sectionName)) {
			const auto& value = cfg.getValue(sectionName, key);
			auto rID = addResource(sectionName, key, value);
			lpg::Log(3, fmt::format("    adding [{}]: {} -> {} [#{}]", sectionName, key, value, rID));
		}
	}
}
void lpg::ResourceManager::registerLoader(const std::string& typeName, lpg::ResourceManager::TypeLoader loader)
{
	loaders[typeName] = loader;
}
lpg::ResourceManager::ResourceID lpg::ResourceManager::addResource(const std::string& type, const std::string& name, const std::string& args)
{
	if(loaders.count(type) == 0) {
		throw ResourceLoaderNotRegistered(fmt::format(
			"cannot load \"{}\" as \"{}\": loader for {} not registered",
			args, name, type
		));
	}
	auto loader = loaders[type];
	if(!loader) {
		throw ResourceLoaderNotRegistered(fmt::format(
			"empty loader given for [{}]",
			type
		));
	}

	
	auto id = resources.emplace(std::unique_ptr<IManagedResource>(loader(args)));
	nameMap[name] = id;
	idMap[id] = name;

	return id;
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


std::string lpg::ResourceManager::getNameOf(lpg::ResourceManager::ResourceID id)
{
	try {
		return idMap.at(id);
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"resource with id={} not found",
			id
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


void lpg::ResourceManager::throwMismatch(const std::type_info& expected, const std::type_info& actual, lpg::ResourceManager::ResourceID id)
{
	throw ResourceTypeMismatch(fmt::format(
		"resource type mismatch: get<{}> called on \"{}\" (id={}), which holds a resource of type {}",
		expected.name(),
		idMap[id],
		id,
		actual.name()
	));
}

lpg::IManagedResource* lpg::ResourceManager::getHandle(lpg::ResourceManager::ResourceID id)
{
	try {
		return (lpg::IManagedResource*)resources.at(id).get();
	} catch(std::out_of_range& e) {
		throw ResourceNotFound(fmt::format(
			"resource with id={} not found",
			id
		));
	}
}

lpg::IManagedResource* DefaultBitmapLoader(const std::string& args)
{
	return dynamic_cast<lpg::IManagedResource*>(
		new lpg::ManagedResource<al::Bitmap>(
			new lpg::ImageFileLoader(args)
		)
	);
}

lpg::IManagedResource* DefaultFontLoader(const std::string& args)
{
	auto vArgs = lpg::str::Split(args, "|");
	for(auto& s: vArgs)
		s = lpg::str::Strip(s);
	
	auto filename = vArgs.at(0);
	double size = 16;
	if(vArgs.size() > 1) {
		size = std::stod(vArgs.at(1));
	}
	return dynamic_cast<lpg::IManagedResource*>(
		new lpg::ManagedResource<al::Font>(
			new lpg::FontFileLoader(filename, size)
		)
	);
}

lpg::IManagedResource* DefaultSVGLoader(const std::string& args)
{
	return dynamic_cast<lpg::IManagedResource*>(
		new lpg::ManagedResource<al::Bitmap>(
			new lpg::SVGLoader(args)
		)
	);
}

void lpg::ResourceManager::registerDefaultLoaders()
{
	registerLoader("Bitmap", DefaultBitmapLoader);
	registerLoader("Font", DefaultFontLoader);
	registerLoader("VectorImage", DefaultSVGLoader);
}


void lpg::ResourceManager::setScale(const al::Coord<>& scale)
{
	for(unsigned i=0; i<resources.size(); i++) {
		if(!resources.contains(i)) {
			continue;
		}

		IManagedResource* mr = getHandle(i);
		if(ScaleAwareLoader* l = dynamic_cast<ScaleAwareLoader*>(mr->getLoader())) {
			l->setScale(scale);
		}
	}
	releaseAllResources();
}
