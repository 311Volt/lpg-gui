#include <lpg/a5xx/Font.hpp>

#include <cstring>
#include <stdexcept>
#include <fmt/format.h>

al::Font::Font(const char* path, int size, int flags)
	: Resource(
		path,
		[&](const char* p)->void*{
			return nullptr;
		},
		MakeDeleter<ALLEGRO_FONT>(al_destroy_font)
	)
{
	this->size = size;
	this->flags = flags;
	loaderFn = [&](const char* p)->void*{
		if(!strcmp(p, "builtin"))
			return al_create_builtin_font();
		return al_load_font(p, this->size, this->flags);
	};
}

al::Font::Font(const char* path, int size)
	: Font(path, size, 0)
{

}

al::Font::Font(const std::string& source)
	: Resource(
		"",
		[&](const char* p)->void*{
			return nullptr;
		},
		MakeDeleter<ALLEGRO_FONT>(al_destroy_font)
	)
{
	char loadingMode = 'A';
	auto cmd = parseSourceString(source);
	try {
		this->sourcePath = cmd.at(0);
		this->size = std::stoi(cmd.at(1));

		if(cmd.size() > 2) {
			loadingMode = cmd.at(2).at(0);
		}

		this->flags = 0;
		if(loadingMode == 'M') {
			this->flags |= ALLEGRO_TTF_MONOCHROME;
		}

		loaderFn = [&](const char* p)->void*{
			if(!strcmp(p, "builtin"))
				return al_create_builtin_font();
			return al_load_font(p, this->size, this->flags);
		};
	}
	catch (std::invalid_argument& e) {
		throw ResourceLoadError(fmt::format(
			"Ill-formed source string ({}). Numerical value "
			"(font size) expected as 1st argument.",
			e.what()
		));
	}
	catch(std::out_of_range& e) {
		throw ResourceLoadError(fmt::format(
			"Ill-formed source string ({}). Provide a size "
			"and optionally a loading mode flag.",
			e.what()
		));
	}
}

al::Font::Font()
	: Font("builtin", 0, 0)
{

}

al::Font::~Font()
{

}

void al::Font::setToFallback()
{
	alPtr.p = al_create_builtin_font();
}

void al::Font::resize(int newSize)
{
	size = newSize * scale;
	reload();
	size = newSize;
}

void al::Font::rescale(float newScale)
{
	scale = newScale;
	resize(size);
}

int al::Font::getSize()
{
	return size;
}

ALLEGRO_FONT* al::Font::ptr()
{
	return (ALLEGRO_FONT*)get();
}
