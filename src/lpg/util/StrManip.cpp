#include <lpg/util/StrManip.hpp>

#include <cctype>
#include <algorithm>

std::string lpg::str::ToLower(const std::string& str)
{
	std::string ret(str);
	std::transform(
		ret.begin(), 
		ret.end(), 
		ret.begin(), 
		[](unsigned char c) -> unsigned char {
			return std::tolower(c);
		}
	);
	return ret;
}

std::string lpg::str::ToUpper(const std::string& str)
{	std::string ret(str);
	std::transform(
		ret.begin(), 
		ret.end(), 
		ret.begin(), 
		[](unsigned char c) -> unsigned char {
			return std::toupper(c);
		}
	);
	return ret;
}
std::vector<std::string> lpg::str::Split(const std::string& str, const std::string& sep)
{
	std::vector<std::string> ret;
	std::string::size_type a = 0;
	while(a < str.length()) {
		std::string::size_type b = str.find(sep);
		if(b == std::string::npos) {
			b = str.length();
		}
		ret.push_back(str.substr(a, b-a));
		a = b + sep.length();
	}
	return ret;
}
std::string lpg::str::Strip(const std::string& str)
{
	std::string::size_type l, r;

	for(l=0; l<str.length(); l++) {
		if(!std::isspace(str[l])) {
			break;
		}
	}

	for(r=str.length(); r>0; r--) {
		if(!std::isspace(str[r-1])) {
			break;
		}
	}

	if(l < r) {
		return std::string(str.begin()+l, str.begin()+r);
	} else {
		return "";
	}
}