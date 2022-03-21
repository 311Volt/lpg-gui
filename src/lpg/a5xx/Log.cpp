#include <lpg/a5xx/Log.hpp>

#include <fmt/format.h>
#include <allegro5/allegro.h>

al::CLog al::Log;


al::CLog::CLog()
{
	verbosity = 0;
	defaultOutput = &std::cerr;
}

void al::CLog::operator()(int minVerbosity, const std::string& msg)
{
	if(verbosity < minVerbosity)
		return;
	(*defaultOutput) << fmt::format(
		"[{0:.3f}] {1}\n",
		al_get_time(),
		msg
	);
}
void al::CLog::SetOutputStream(std::ostream& os)
{
	defaultOutput = &os;
}
void al::CLog::SetVerbosity(int level)
{
	verbosity = level;
}
