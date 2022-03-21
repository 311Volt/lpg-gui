#ifndef LPG_AL_CONFIG_H
#define LPG_AL_CONFIG_H

#include <string>
#include <unordered_map>
#include <allegro5/allegro.h>
#include <vector>

namespace al {
	class Config {
	public:
		Config();
		Config(const char* path);
		Config(const std::string& path);
		~Config();

		std::string get(const std::string& section, const std::string& key);
		std::string get(const std::string& key);

		void set(const std::string& section, const std::string& key, const std::string& value);
		void set(const std::string& key, const std::string& value);

		bool remove(const std::string& section, const std::string& key);
		bool remove(const std::string& key);

		bool removeSection(const std::string& name);
		void addSection(const std::string& name);

		bool saveToDisk();
		bool saveToDisk(const std::string& filename);

		std::vector<std::string> getAllSections();
		std::vector<std::string> getAllSectionKeys(const std::string& section);

	private:
		std::string sourcePath;
		ALLEGRO_CONFIG* alPtr;
	};
}

#endif // LPG_AL_CONFIG_H
