#include <lpg/gui/Image.hpp>

#include <cmath>

gui::Image::Image(const std::string& resName, float x, float y)
	: Window(0, 0, x, y)
{
        setTo(resName);
}

void gui::Image::setTo(const std::string& resName)
{
	this->resName = resName;
	rID = RM.getIdOf(resName);

	std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID);
	resize(ToUnits({bmp->getWidth(), bmp->getHeight()}));
}

void gui::Image::render()
{
	if(!visible)
		return;

	std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID);
	bmp->drawScaled(bmp->getRect(), getScreenRectangle());
}

