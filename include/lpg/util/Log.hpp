#ifndef INCLUDE_LPG_UTIL_LOG
#define INCLUDE_LPG_UTIL_LOG

#include <iostream>
#include <format>

namespace lpg {

	class CLog {
	public:
		CLog();

		void operator()(int minVerbosity, const std::string& msg);
		void SetOutputStream(std::ostream& os);
		void SetVerbosity(int level);
	private:
		int verbosity;
		std::ostream* defaultOutput;
	};
	extern CLog Log;
}


#endif /* INCLUDE_LPG_UTIL_LOG */
