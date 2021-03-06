#include <lpg/gui/Image.hpp>

#include <cmath>

gui::Image::Image(const std::string& resName, al::Coord<> pos)
	: Window({1,1}, pos)
{
	setTo(resName);
	setTitle("An image");
}

void gui::Image::setTo(const std::string& resName)
{
	this->resName = resName;
	rID = RM.getIdOf(resName);

	std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID);
	resize(ToUnits(bmp->size()));
}

void gui::Image::render()
{
	std::shared_ptr<al::Bitmap> bmp = RM.get<al::Bitmap>(rID);
	bmp->drawScaled(bmp->rect(), {{0,0}, getScreenSize().floor()});
}

