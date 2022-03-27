#include <lpg/resmgr/DefaultLoaders.hpp>

lpg::ImageFileLoader::ImageFileLoader(const std::string& filename)
	: filename(filename)
{

}
al::Bitmap* lpg::ImageFileLoader::createObject()
{
	return new al::Bitmap(filename);
}


lpg::FontFileLoader::FontFileLoader(const std::string& filename, int size)
	: filename(filename), size(size), scale{1.0f, 1.0f}
{

}
al::Font* lpg::FontFileLoader::createObject()
{
	int actualSize = size * scale.x;
	return new al::Font(filename, actualSize);
}
void lpg::FontFileLoader::setScale(al::Vec2 scale)
{
	this->scale = scale;
}