#ifndef INCLUDE_LPG_RESMGR_DEFAULTLOADERS
#define INCLUDE_LPG_RESMGR_DEFAULTLOADERS

#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/resources/Font.hpp>
#include <axxegro/math/math.hpp>

#include <lpg/resmgr/ResourceLoader.hpp>

namespace lpg {
	class ImageFileLoader: public ResourceLoader<al::Bitmap> {
		std::string filename;
	public:
		ImageFileLoader(const std::string& filename);
		virtual al::Bitmap* createObject() override;
	};

	class FontFileLoader: public ResourceLoader<al::Font>, public ScaleAwareLoader {
		std::string filename;
		int size;
		al::Vec2 scale;
	public:
		FontFileLoader(const std::string& filename, int size);
		virtual al::Font* createObject() override;
		virtual void setScale(al::Vec2 scale) override;
	};
}

#endif /* INCLUDE_LPG_RESMGR_DEFAULTLOADERS */
