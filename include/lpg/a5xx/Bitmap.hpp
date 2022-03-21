#ifndef LPG_AL_BITMAP_H
#define LPG_AL_BITMAP_H

#include "Resource.hpp"

#include <allegro5/allegro.h>

namespace al {

	class Bitmap : public Resource {
	public:
		Bitmap(int w, int h);
		Bitmap(const char* filename);
		~Bitmap();

		virtual void setToFallback() override;

		ALLEGRO_BITMAP* ptr();
	protected:

	};
}

#endif // LPG_AL_BITMAP_H
