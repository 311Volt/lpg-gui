#ifndef LPG_AL_LOG_H
#define LPG_AL_LOG_H

#include <iostream>
#include <fmt/format.h>

namespace al {

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

#endif // LPG_AL_LOG_H
