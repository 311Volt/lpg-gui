#ifndef LPG_AL_RAII_H
#define LPG_AL_RAII_H

#include <allegro5/allegro5.h>

/* note: when another one of those is needed, make this into a template */

namespace al {

	class ScopedTargetBitmap {
		const ALLEGRO_BITMAP* originalTarget;
	public:
		ScopedTargetBitmap(ALLEGRO_BITMAP* bmp)
			: originalTarget(al_get_target_bitmap())
		{
			al_set_target_bitmap(bmp);
		}

		~ScopedTargetBitmap()
		{
			al_set_target_bitmap(const_cast<ALLEGRO_BITMAP*>(originalTarget));
		}
	};

	class ScopedTransform {
		const ALLEGRO_TRANSFORM* originalTransform;
	public:
		ScopedTransform(ALLEGRO_TRANSFORM* t)
			: originalTransform(al_get_current_transform())
		{
			al_use_transform(t);
		}

		~ScopedTransform()
		{
			al_use_transform(const_cast<ALLEGRO_TRANSFORM*>(originalTransform));
		}
	};
}

#endif // LPG_AL_RAII_H
