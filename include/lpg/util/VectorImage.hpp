#ifndef LPG_VECTORIMAGE_H
#define LPG_VECTORIMAGE_H

#include <axxegro/resources/ResourceHandle.hpp>
#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/math/math.hpp>
#include <axxegro/Color.hpp>

#include <lunasvg.h>
#include <string>
#include <memory>

namespace lpg {
	class VectorImage : public al::ResourceHandle<al::Bitmap> {
	public:
		VectorImage(const std::string& path);
		VectorImage(const std::string& path, al::Vec2 scale, al::Color col);

		virtual void load() override;

		al::Vec2 getRenderDimensions();
		void rescale(al::Vec2 newScale);
		void resize(al::Vec2 newSize);
		void changeBgColor(al::Color col);


	private:
		void render();
		al::Color bgColor;
		std::unique_ptr<lunasvg::Document> doc;
		al::Vec2 scale;


	// 	VectorImage(const char* path, int rw, int rh, ALLEGRO_COLOR bgCol);
	// 	VectorImage(const std::string& source);
	// 	~VectorImage();

	// 	int getRenderWidth();
	// 	int getRenderHeight();

	// 	void rescale(float newScale);
	// 	void resize(int w, int h);
	// 	void setBgColor(ALLEGRO_COLOR color);

	// 	static constexpr int MaxWidth = 65535;
	// 	static constexpr int MaxHeight = 65535;

	// protected:
	// 	void defaultInit();

	// 	std::unique_ptr<lunasvg::Document> document;

	// 	void renderDocumentToBitmap(ALLEGRO_BITMAP* bitmap);
	// 	void* svgLoad(const char* path);
	// 	void svgUnload();

	// 	bool keepDocument;
	// 	int baseWidth, baseHeight;
	// 	float renderScale;
	// 	ALLEGRO_COLOR bgColor;
	};
}

#endif // LPG_VECTORIMAGE_H
