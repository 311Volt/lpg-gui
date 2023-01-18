#include <lpg/resmgr/SVGLoader.hpp>

#include <memory>
#include <utility>

#include <lpg/util/Log.hpp>

#include <format>
#include <allegro5/allegro.h>


lpg::SVGLoader::SVGLoader(const std::string& filename)
	: filename(filename), scale{1.0f, 1.0f}
{
	document = lunasvg::Document::loadFromFile(filename);
	if(!document) {
		throw al::ResourceLoadError("Could not load SVG file from \""+filename+"\"");
	}
}

inline uint32_t lunaAlphaPremul(uint32_t col)
{
	uint16_t r,g,b,a;
	r = (col>>0)&0xFF;
	g = (col>>8)&0xFF;
	b = (col>>16)&0xFF;
	a = (col>>24)&0xFF;

	r = (r*a + 255) >> 8;
	g = (g*a + 255) >> 8;
	b = (b*a + 255) >> 8;

	return (r<<0) + (g<<8) + (b<<16) + (a<<24);
}

al::Bitmap* lpg::SVGLoader::createObject()
{
	int targetWidth = scale.x * document->width();
	int targetHeight = scale.y * document->height();

	lpg::Log(3, std::format("creating a {}x{} bitmap from {}\n", targetWidth, targetHeight, filename));


	lunasvg::Bitmap render = document->renderToBitmap(targetWidth, targetHeight);
	if(!render.valid()) {
		throw al::ResourceLoadError(std::format(
			"Error while rendering SVG (loaded from \"{}\") to a {}x{} bitmap",
			filename,
			targetWidth, targetHeight
		));
	}
	
	std::unique_ptr<al::Bitmap> ret = std::make_unique<al::Bitmap>(targetWidth, targetHeight);
	ret->clearToColor(al::RGBA(0,0,0,0));
	{
		al::BitmapLockedRegion lr(*ret, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

		for(int y=0; y<targetHeight; y++) {
			uint32_t* srcLine = (uint32_t*)(render.data() + 4*targetWidth*y);
			uint32_t* dstLine = lr.rowData<uint32_t>(y);

			for(int x=0; x<targetWidth; x++) {
				uint32_t tmp = lunaAlphaPremul(srcLine[x]);
				dstLine[x] = tmp;
			}
			//std::transform(srcLine, srcLine+targetWidth, dstLine, lunaAlphaPremul);
		}
	}

	return ret.release();
}

void lpg::SVGLoader::setScale(const al::Vec2<>& scale)
{
	this->scale = scale;
}