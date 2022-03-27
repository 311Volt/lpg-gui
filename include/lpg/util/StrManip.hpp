#ifndef INCLUDE_LPG_UTIL_STRMANIP
#define INCLUDE_LPG_UTIL_STRMANIP

/**
 * @brief 
 * Provides string operations like splitting or stripping.
 */

#include <vector>
#include <string>

namespace lpg {
	namespace str {
		std::string ToLower(const std::string& str);
		std::string ToUpper(const std::string& str);
		std::vector<std::string> Split(const std::string& str, const std::string& sep);
		std::string Strip(const std::string& str);

		template<typename I>
		std::string Join(I begin, I end)
		{
			std::string ret;
			for(I i=begin; i!=end; i++) {
				ret += *i;
			}
			return ret;
		}
	}
}

#endif /* INCLUDE_LPG_UTIL_STRMANIP */
