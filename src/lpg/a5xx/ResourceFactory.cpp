#include <lpg/a5xx/Resource.hpp>
#include <lpg/a5xx/ResourceFactory.hpp>

#include <filesystem>
#include <stdexcept>
namespace fs = std::filesystem;

std::unordered_map<std::string, al::ResourceFactory::LoaderFnT> al::ResourceFactory::Loaders;
std::unordered_map<std::string, std::vector<std::string>> al::ResourceFactory::Extensions;

std::string al::ResourceFactory::TypeName(const std::string& source)
{
	std::string ext = fs::path(Resource::parseSourceString(source)[0]).extension().u8string();
	for(auto& [typeName, extVec]: Extensions) {
		if(std::find(extVec.begin(), extVec.end(), ext) != extVec.end()) {
			return typeName;
		}
	}
	return "";
}

al::ResourceFactory::ResPtrT al::ResourceFactory::Create(const std::string& source)
{
	throw std::runtime_error("not implemented");
	return nullptr;
}

al::ResourceFactory::ResPtrT al::ResourceFactory::CreateAs(const std::string& type, const std::string& source)
{
	throw std::runtime_error("not implemented");
	return nullptr;
}

void al::ResourceFactory::AddExtension(const std::string& name, const std::string& ext)
{
	if(Extensions.count(name)) {
		Extensions[name].push_back(ext);
	}
}
