
#include <lpg/util/ResourceManager.hpp>

#include <lpg/a5xx/Bitmap.hpp>
#include <lpg/a5xx/Font.hpp>
#include <lpg/a5xx/Sample.hpp>
#include <lpg/util/VectorImage.hpp>


lpg::ResourceManager::ResourceManager()
{
	releaseTime = 25.0;
}

lpg::ResourceManager::~ResourceManager()
{

}

bool lpg::ResourceManager::loadFromConfig(const std::string& path)
{
	al::Config cfg(path);

	for(auto& key: cfg.getAllSectionKeys("Bitmap")) {
		nameMap[key] = idMap.emplace(std::make_unique<al::Bitmap>(cfg.get("Bitmap", key).c_str()));
	}
	for(auto& key: cfg.getAllSectionKeys("VectorImage")) {
		nameMap[key] = idMap.emplace(std::make_unique<lpg::VectorImage>(cfg.get("VectorImage", key).c_str()));
	}
	for(auto& key: cfg.getAllSectionKeys("Font")) {
		nameMap[key] = idMap.emplace(std::make_unique<al::Font>(cfg.get("Font", key).c_str()));
	}
	for(auto& key: cfg.getAllSectionKeys("Sample")) {
		nameMap[key] = idMap.emplace(std::make_unique<al::Sample>(cfg.get("Sample", key).c_str()));
	}

	//TODO wtf is this
	return true;
}

uint32_t lpg::ResourceManager::give(std::unique_ptr<al::Resource> res, const std::string& name)
{
	uint32_t id = idMap.emplace(std::move(res));
	nameMap[name] = id;
	return id;
}

void lpg::ResourceManager::releaseUnusedResources()
{
	for(uint32_t i=0; i<idMap.size(); i++) {
		if(!idMap.contains(i)) {
			continue;
		}
		auto& resPtr = idMap[i];
		if(resPtr->timeSinceLastUse() > releaseTime) {
			resPtr->unload();
		}
	}
}

void lpg::ResourceManager::clear()
{
	nameMap.clear();
	idMap.clear();
}

al::Resource* lpg::ResourceManager::getPtr(const std::string& name)
{
	al::Resource* ret = nullptr;
	try {
		ret = idMap.at(nameMap.at(name)).get();
		ret->get();
	} catch(std::out_of_range& e) {
		throw std::out_of_range(fmt::format(
			"ResMgr: cannot find resource \"{}\"",
			name
		));
	}
	return ret;
}
