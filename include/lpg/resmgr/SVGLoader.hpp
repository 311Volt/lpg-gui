#ifndef INCLUDE_LPG_RESMGR_SVGLOADER
#define INCLUDE_LPG_RESMGR_SVGLOADER

#include <lpg/resmgr/ResourceLoader.hpp>
#include <axxegro/resources/Bitmap.hpp>

#include <lunasvg.h>

namespace lpg {
	class SVGLoader: public ResourceLoader<al::Bitmap>, public ScaleAwareLoader {
	public:
		SVGLoader(const std::string& filename);
		virtual al::Bitmap* createObject() override;
		virtual void setScale(al::Vec2 scale) override;
	private:
		std::string filename;
		std::unique_ptr<lunasvg::Document> document;
		al::Vec2 scale;
	};
}

#endif /* INCLUDE_LPG_RESMGR_SVGLOADER */
