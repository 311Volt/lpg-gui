#include <lpg/util/Log.hpp>

#include <axxegro/time/Time.hpp>
#include <format>

lpg::CLog lpg::Log;


lpg::CLog::CLog()
{
	verbosity = 0;
	defaultOutput = &std::cerr;
}

void lpg::CLog::operator()(int minVerbosity, const std::string& msg)
{
	if(verbosity < minVerbosity)
		return;
	(*defaultOutput) << std::format(
		"[{0:.3f}] {1}\n",
		al::GetTime(),
		msg
	);
}
void lpg::CLog::SetOutputStream(std::ostream& os)
{
	defaultOutput = &os;
}
void lpg::CLog::SetVerbosity(int level)
{
	verbosity = level;
}