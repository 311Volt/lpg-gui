#include <lpg/gui/Checkbox.hpp>

gui::Checkbox::Checkbox(al::Vec2<> size, al::Coord<> pos)
	: Button(size, pos, "", [this](){checked=!checked;}),
	checkmark("Checkmark", {0, 0})
{
	setTitle("");
	bgColor = al::RGB(255, 255, 255);
	edgeType = EDGE_BEVELED_INWARD;

	checked = false;

	checkmark.resize(size);
	addChild(checkmark);
	updateAppearance();
}

gui::Checkbox::Checkbox(al::Coord<> pos)
	: Checkbox(DefaultSize, pos)
{

}

void gui::Checkbox::updateAppearance()
{
	edgeType = EDGE_BEVELED_INWARD; //to override Button::onDefault() behavior
	bgColor = (state==State::DOWN) ? al::RGB(224,224,224) : al::RGB(255,255,255);
	checkmark.visible = checked;
}
