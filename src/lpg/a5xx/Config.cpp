#include <lpg/a5xx/Config.hpp>

al::Config::Config()
{
	sourcePath = "";
	alPtr = al_create_config();
}

al::Config::Config(const char* path)
{
	sourcePath = std::string(path);
	alPtr = al_load_config_file(path);
}

al::Config::Config(const std::string& path)
	: Config(path.c_str())
{

}

al::Config::~Config()
{
	saveToDisk();
	al_destroy_config(alPtr);
}

std::string al::Config::get(const std::string& section, const std::string& key)
{
	return std::string(al_get_config_value(alPtr, section.c_str(), key.c_str()));
}

std::string al::Config::get(const std::string& key)
{
	return get("", key);
}

void al::Config::set(const std::string& section, const std::string& key, const std::string& value)
{
	al_set_config_value(alPtr, section.c_str(), key.c_str(), value.c_str());
}

void al::Config::set(const std::string& key, const std::string& value)
{
	set("", key, value);
}

bool al::Config::remove(const std::string& section, const std::string& key)
{
	return al_remove_config_key(alPtr, section.c_str(), key.c_str());
}

bool al::Config::remove(const std::string& key)
{
	return remove("", key);
}

bool al::Config::removeSection(const std::string& name)
{
	return al_remove_config_section(alPtr, name.c_str());
}

void al::Config::addSection(const std::string& name)
{
	al_add_config_section(alPtr, name.c_str());
}

bool al::Config::saveToDisk()
{
	if(sourcePath != "") {
		return saveToDisk(sourcePath);
	}
	return false;
}

bool al::Config::saveToDisk(const std::string& filename)
{
	return al_save_config_file(filename.c_str(), alPtr);
}

std::vector<std::string> al::Config::getAllSections()
{
	ALLEGRO_CONFIG_SECTION* it;
	std::vector<std::string> ret;
	auto fst = al_get_first_config_section(alPtr, &it);
	if(fst) {
		ret.push_back(std::string(fst));
		while(it) {
			auto entry = al_get_next_config_section(&it);
			if(entry) {
				ret.push_back(entry);
			}
		}
	}
	return ret;
}

std::vector<std::string> al::Config::getAllSectionKeys(const std::string& section)
{
	ALLEGRO_CONFIG_ENTRY* it;
	std::vector<std::string> ret;
	auto fst = al_get_first_config_entry(alPtr, section.c_str(), &it);
	if(fst) {
		ret.push_back(std::string(fst));
		while(it) {
			auto entry = al_get_next_config_entry(&it);
			if(entry) {
				ret.push_back(entry);
			}
		}
	}
	return ret;
}
