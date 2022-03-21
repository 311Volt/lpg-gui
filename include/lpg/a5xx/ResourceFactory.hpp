#ifndef LPG_AL_RESOURCE_FACTORY_H
#define LPG_AL_RESOURCE_FACTORY_H

#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

namespace al {
	class Resource;

	class ResourceFactory {
	public:
		using ResPtrT = std::unique_ptr<Resource>;
		using LoaderFnT = std::function<ResPtrT(const std::string&)>;

		static std::string TypeName(const std::string& source);
		static ResPtrT Create(const std::string& source);
		static ResPtrT CreateAs(const std::string& type, const std::string& source);

		void AddExtension(const std::string& name, const std::string& ext);

		template<class T>
		static void RegisterType(const std::string& name)
		{
			Loaders[name] = [](const std::string& src) {
				return ResPtrT(static_cast<Resource*>(new T(src)));
			};
		}
	private:
		static std::unordered_map<std::string, LoaderFnT> Loaders;
		static std::unordered_map<std::string, std::vector<std::string>> Extensions;
	};
}

#endif // LPG_AL_RESOURCE_FACTORY_H
