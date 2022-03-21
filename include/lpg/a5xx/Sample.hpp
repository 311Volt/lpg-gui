#ifndef LPG_AL_SAMPLE_H
#define LPG_AL_SAMPLE_H

#include "Resource.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

#include <string>

namespace al {

	class Sample : public Resource {
	protected:
	public:
		Sample(const char* filename);
		Sample(const std::string& filename);
		~Sample();

		virtual void setToFallback() override;

		ALLEGRO_SAMPLE* ptr();

	};
}

#endif // LPG_AL_BITMAP_H
