#include <lpg/util/VectorImage.hpp>

#include <fmt/format.h>
#include <algorithm>
#include <iostream>
#include <filesystem>

#include <lpg/a5xx/Log.hpp>
#include <lpg/a5xx/Resource.hpp>


void lpg::VectorImage::defaultInit()
{
	renderScale = 1.0;
	bgColor = al_map_rgba(0,0,0,0);
	reloadable = true;
	keepDocument = true;
	status = Status::UNLOADED;


	loaderFn = [&](const char* path) -> void* {
		return svgLoad(path);
	};

	deleteFn = [&](void* ptr) -> void {
		svgUnload();
		al_destroy_bitmap((ALLEGRO_BITMAP*)ptr);
	};
}

lpg::VectorImage::VectorImage(const char* path, int rw, int rh, ALLEGRO_COLOR bgCol)
	: Bitmap(path)
{
	defaultInit();

	baseWidth = rw;
	baseHeight = rh;
	bgColor = al_map_rgba(0,0,0,0);
}

lpg::VectorImage::VectorImage(const std::string& source)
	: Bitmap("")
{
	auto cmd = parseSourceString(source);
	try {
		this->sourcePath = cmd.at(0);
		std::string res = cmd.at(1);

		//
		auto x = std::find(res.begin(), res.end(), 'x');
		std::string srw(res.begin(), x);
		std::string srh(x + !!(x!=res.end()), res.end());

		try {
			baseWidth = std::stoi(srw);
			baseHeight = std::stoi(srh);
		} catch(std::invalid_argument& e) {
			throw al::ResourceLoadError(fmt::format(
				"Could not convert \"{}\" and \"{}\" into width and height values",
				srw,
				srh
			));
		}

		if(baseWidth < 1 || baseWidth > MaxHeight) {
			throw al::ResourceLoadError(fmt::format(
				"width must be a number in the range 1-{} (got {})",
				MaxHeight,
				srw
			));
		}
		if(baseHeight < 1 || baseHeight > MaxHeight) {
			throw al::ResourceLoadError(fmt::format(
				"height must be a number in the range 1-{} (got {})",
				MaxHeight,
				srw
			));
		}


	} catch(std::out_of_range& e) {
		throw al::ResourceLoadError(fmt::format(
			"Ill-formed source string. Make sure to specify the resolution (e. g. \"800x600\") when loading an SVG file. ({})",
			e.what()
		));
	}

	defaultInit();
}

lpg::VectorImage::~VectorImage()
{
	/* this is necessary because the default deleteFn accesses
	 * an object that is no longer valid by the time the body of
	 * ~Resource() is reached */
	deleteFn = al::MakeDeleter<ALLEGRO_BITMAP>(al_destroy_bitmap);
}

int lpg::VectorImage::getRenderWidth()
{
	return baseWidth * renderScale;
}

int lpg::VectorImage::getRenderHeight()
{
	return baseHeight * renderScale;
}

void lpg::VectorImage::rescale(float newScale)
{
	renderScale = newScale;
	resize(baseWidth, baseHeight); //for side effect of reloading
}

void lpg::VectorImage::resize(int w, int h)
{
	if(w < 1 || w > MaxWidth || h < 1 || h > MaxHeight) {
		throw std::runtime_error(fmt::format(
			"lpg::VectorImage::resize() called with unexpected arguments ({:d},{:d})",
			w, h
		));
	}

	baseWidth = w;
	baseHeight = h;

	int rw = getRenderHeight();
	int rh = getRenderWidth();
	if(document) {
		al_destroy_bitmap((ALLEGRO_BITMAP*)alPtr.p);
		alPtr.p = (ALLEGRO_BITMAP*)al_create_bitmap(rw, rh);
		renderDocumentToBitmap((ALLEGRO_BITMAP*)alPtr.p);
	}
	else {
		reload();
	}
}

void lpg::VectorImage::setBgColor(ALLEGRO_COLOR color)
{
	bgColor = color;
}

inline uint32_t allegroToLuna(ALLEGRO_COLOR col)
{
	uint32_t ret = 0;
	uint8_t r,g,b,a;
	al_unmap_rgba(col, &r, &g, &b, &a);
	ret += uint32_t(a) << 24;
	ret += uint32_t(r) << 16;
	ret += uint32_t(g) << 8;
	ret += uint32_t(b) << 0;
	return ret;
}

inline uint32_t lunaAlphaPremul(uint32_t col)
{
	uint32_t r,g,b,a;
	r = (col>>0)&0xFF;
	g = (col>>8)&0xFF;
	b = (col>>16)&0xFF;
	a = (col>>24)&0xFF;

	r = (r*a + 255) >> 8;
	g = (g*a + 255) >> 8;
	b = (b*a + 255) >> 8;

	return (r<<0) + (g<<8) + (b<<16) + (a<<24);
}

void lpg::VectorImage::renderDocumentToBitmap(ALLEGRO_BITMAP* bitmap)
{
	ALLEGRO_LOCKED_REGION* reg = al_lock_bitmap(
		bitmap,
		ALLEGRO_PIXEL_FORMAT_ABGR_8888,
		ALLEGRO_LOCK_WRITEONLY
	);

	if(!reg) {
		throw al::ResourceLoadError("Failed to lock the Allegro bitmap for writing");
	}

	int rw = al_get_bitmap_width(bitmap);
	int rh = al_get_bitmap_height(bitmap);

	uint32_t u32bgColor = allegroToLuna(bgColor);
	double renderTime = al_get_time();
	lunasvg::Bitmap lbmp = document->renderToBitmap(rw, rh, u32bgColor);
	renderTime = al_get_time() - renderTime;

	double copyTime = al_get_time();
	if(!lbmp.valid()) {
		throw al::ResourceLoadError(fmt::format(
			"Failed to render document to a {}x{} bitmap (bgcolor=#{:06X}:{})",
			rw,
			rh,
			u32bgColor,
			u32bgColor>>24
		));
	}

	for(int y=0; y<rh; y++) {
		/* Assuming lbmp.data() points to the rendered image
		 * in ABGR_8888 without padding. Watch out for this
		 * bit when porting? */

		uint32_t* srcLine = (uint32_t*)(lbmp.data() + 4*rw*y);
		uint32_t* dstLine = (uint32_t*)((uint8_t*)reg->data + y*reg->pitch);

		std::transform(srcLine, srcLine+rw, dstLine, lunaAlphaPremul);
	}
	al_unlock_bitmap(bitmap);
	copyTime = al_get_time() - copyTime;

	al::Log(3, fmt::format(
		"svg loader: render: {:.3f}ms ({:.1f}%), copy: {:.3f}ms ({:.1f}%)",
		1000.0*renderTime,
		100.0 * (renderTime / (renderTime + copyTime)),
		1000.0*copyTime,
		100.0 * (copyTime / (renderTime + copyTime))
	));
}

void* lpg::VectorImage::svgLoad(const char* path)
{
	document = lunasvg::Document::loadFromFile(path);
	if(!document) {
		throw al::ResourceLoadError(fmt::format(
			"File not found or document corrupted"
		));
	}

	int rw = getRenderWidth();
	int rh = getRenderHeight();

	ALLEGRO_BITMAP* ret = al_create_bitmap(rw, rh);
	renderDocumentToBitmap(ret);

	return ret;
}

void lpg::VectorImage::svgUnload()
{
	if(!keepDocument)
		document.reset(nullptr);
}
