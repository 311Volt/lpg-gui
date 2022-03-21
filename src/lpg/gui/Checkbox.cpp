#include <lpg/gui/Checkbox.hpp>

gui::Checkbox::Checkbox(float x, float y, float w, float h)
	: Button(w, h, x, y, [this](){checked=!checked;}),
	checkmark("Checkmark", 0, 0)
{
	setTitle("");
	bgColor = al_map_rgb(255, 255, 255);
	edgeType = EdgeType::BEVELED_INWARD;

	checked = false;

	checkmark.resize(w, h);
	addChild(checkmark);
	updateAppearance();
}

gui::Checkbox::Checkbox(float x, float y)
	: Checkbox(x, y, DefaultWidth, DefaultHeight)
{

}

void gui::Checkbox::updateAppearance()
{
	edgeType = EdgeType::BEVELED_INWARD; //to override Button::onDefault() behavior
	bgColor = (state==State::DOWN) ? al_map_rgb(224,224,224) : al_map_rgb(255,255,255);
	checkmark.visible = checked;
}

gui::Checkbox::~Checkbox()
{

}
