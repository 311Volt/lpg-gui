#ifndef LPG_AL_FONT_H
#define LPG_AL_FONT_H

#include "Resource.hpp"

#include <allegro5/allegro_ttf.h>
#include <string_view>

namespace al {
	class Font : public Resource {
	public:
		Font(const char* path, int size, int flags);
		Font(const char* path, int size);
		Font(const std::string& source);
		Font();
		~Font();

		virtual void setToFallback() override;
		void resize(int newSize);
		void rescale(float newScale);
		int getSize();

		ALLEGRO_FONT* ptr();
	protected:
		float scale;
		int size, flags;
	};
}

#endif // LPG_AL_FONT_H
