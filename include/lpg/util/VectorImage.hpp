#ifndef LPG_VECTORIMAGE_H
#define LPG_VECTORIMAGE_H

#include "../a5xx/Bitmap.hpp"
#include <lunasvg.h>
#include <string>

namespace lpg {
	class VectorImage : public al::Bitmap {
	public:
		VectorImage(const char* path, int rw, int rh, ALLEGRO_COLOR bgCol);
		VectorImage(const std::string& source);
		~VectorImage();

		int getRenderWidth();
		int getRenderHeight();

		void rescale(float newScale);
		void resize(int w, int h);
		void setBgColor(ALLEGRO_COLOR color);

		static constexpr int MaxWidth = 65535;
		static constexpr int MaxHeight = 65535;

	protected:
		void defaultInit();

		std::unique_ptr<lunasvg::Document> document;

		void renderDocumentToBitmap(ALLEGRO_BITMAP* bitmap);
		void* svgLoad(const char* path);
		void svgUnload();

		bool keepDocument;
		int baseWidth, baseHeight;
		float renderScale;
		ALLEGRO_COLOR bgColor;
	};
}

#endif // LPG_VECTORIMAGE_H
