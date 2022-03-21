#include <lpg/gui/Image.hpp>
#include <lpg/util/VectorImage.hpp>

#include <cmath>

gui::Image::Image(const std::string& resName, float x, float y)
	: Window(0, 0, x, y)
{
        setTo(resName);
        visible = true;
}

void gui::Image::setTo(const std::string& resName)
{
	this->resName = resName;
	bmp = RM.get<al::Bitmap>(resName);

	Point imgSize(al_get_bitmap_width(bmp->ptr()), al_get_bitmap_height(bmp->ptr()));
	resize(ToUnits(imgSize));
}

void gui::Image::render()
{
	if(!visible)
		return;

	Point imgSize(al_get_bitmap_width(bmp->ptr()), al_get_bitmap_height(bmp->ptr()));
	Point scrSize = getScreenSize();
	scrSize.x = std::floor(scrSize.x);
	scrSize.y = std::floor(scrSize.y);
	al_draw_scaled_bitmap(bmp->ptr(), 0, 0, imgSize.x, imgSize.y, 0, 0, scrSize.x, scrSize.y, 0);
}

void gui::Image::onRescale()
{
	if(auto* vImg = dynamic_cast<lpg::VectorImage*>(bmp)) {
		vImg->rescale(GetEnvScale());
	}
}
