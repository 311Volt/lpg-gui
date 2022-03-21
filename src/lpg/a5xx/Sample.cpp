#include <lpg/a5xx/Sample.hpp>

#include <array>
#include <cstdint>
#include <algorithm>

al::Sample::Sample(const char* filename)
	: Resource(
		filename,
		MakeLoader<ALLEGRO_SAMPLE>(al_load_sample),
		MakeDeleter<ALLEGRO_SAMPLE>(al_destroy_sample)
	)
{

}

al::Sample::Sample(const std::string& filename)
	: Sample(filename.c_str())
{

}

al::Sample::~Sample()
{

}

const size_t FALLBACK_SAMPLE_SIZE = 256;
const int FALLBACK_SAMPLE_FREQ = 44100;

void al::Sample::setToFallback()
{
	uint16_t* buf = (uint16_t*)al_calloc(FALLBACK_SAMPLE_SIZE, sizeof(uint16_t));
	alPtr.p = al_create_sample(
		buf,
		FALLBACK_SAMPLE_SIZE,
		FALLBACK_SAMPLE_FREQ,
		ALLEGRO_AUDIO_DEPTH_INT16,
		ALLEGRO_CHANNEL_CONF_1,
		true
	);
	//reminder: the sample structure now owns the buffer, hence no free()
}

ALLEGRO_SAMPLE* al::Sample::ptr()
{
	return (ALLEGRO_SAMPLE*)get();
}
