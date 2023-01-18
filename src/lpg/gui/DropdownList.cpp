
#include <lpg/gui/DropdownList.hpp>

gui::DropdownList::DropdownList(al::Vec2<> size, al::Coord<> pos, al::Vec2<> listSize)
	: Window(size, pos),
	  listSize(listSize),
	  elementListWin(*this), 
	  downBtn({16, getHeight()-4}, {-2,0}, "v"),
	  text(size, {0,0}, ""), vPos(0), selectedIdx(0),
	  focusedIdx(0)

{
	elementListWin.setPos({0, getHeight()});
	setBgColor(al::White);
	setEdgeType(EDGE_BEVELED_INWARD);

	registerEventHandler(ALLEGRO_EVENT_MOUSE_BUTTON_DOWN, &DropdownList::onMouseDown);

	downBtn.setAlignment(ALIGN_RIGHT_CENTER);
	text.setTextAlignment(ALIGN_LEFT_CENTER);

	addChild(text);
	addChild(elementListWin);
	addChild(downBtn);
}

void gui::DropdownList::setOnChangeCallback(OnChangeFn fn)
{
	onChange = fn;
}

void gui::DropdownList::append(const ElementData& dat)
{
	auto& v = elementListWin.elements;

	v.emplace_back(std::make_unique<Element>(
		al::Coord<>{getWidth(), elementListWin.elementHeight}, 
		dat, 
		v.size(), 
		[this](){
			elementListWin.visible = false;
			changeSelection(focusedIdx);
		}
	));

	v.back()->onHoverCallback = [&](uint32_t i){
		focusedIdx = i;
	};

	elementListWin.addChild(*v.back());
}
void gui::DropdownList::setElements(const std::vector<ElementData>& data)
{
	elementListWin.elements.clear();
	for(const auto& d: data) {
		append(d);
	}
	changeSelection(0);
}

bool gui::DropdownList::changeSelection(uint32_t idx)
{
	if(idx >= elementListWin.elements.size()) {
		return false;
	}
	auto* el = elementListWin.elements[idx].get();
	if(!el) {
		throw std::runtime_error("dropdown list element is null (this should never happen)");
	}

	selectedIdx = idx;
	text.setText(el->getData().text);
	if(onChange) {
		onChange(getCurrentSelectionId());
	}
	return true;
}

void gui::DropdownList::onMouseDown(const ALLEGRO_EVENT& ev)
{
	if(IsClickEvent(ev, MouseBtn::LEFT, getScreenRectangle())) {
		elementListWin.visible = !elementListWin.visible;
	} else {
		elementListWin.handleEvent(ev);
	}
}

const gui::DropdownList::ElementData& gui::DropdownList::getCurrentSelectionData()
{
	return elementListWin.elements.at(selectedIdx)->getData();
}

uint32_t gui::DropdownList::getCurrentSelectionId()
{
	return getCurrentSelectionData().id;
}
uint32_t gui::DropdownList::getCurrentSelectionIndex()
{
	return selectedIdx;
}

void gui::DropdownList::tick()
{
	Window::tick();
}
void gui::DropdownList::render()
{
	Window::render();
}


gui::DropdownList::Element::Element(al::Vec2<> size, ElementData dat, uint32_t idx, CallbackT callback)
	: Button(size, {0,0}, "", callback), dat(dat), idx(idx)
{
	setTitle(dat.text);
	caption.setTextColor(al::White);
	setBgColor(al::DarkGray);
	caption.setTextAlignment(ALIGN_LEFT_CENTER);
	caption.setPadding({2,4,2,2});

	updateAppearance();
}

const gui::DropdownList::ElementData& gui::DropdownList::Element::getData()
{
	return dat;
}

void gui::DropdownList::Element::onHover()
{
	Button::onHover();
	if(onHoverCallback) {
		onHoverCallback(idx);
	}
}

void gui::DropdownList::Element::updateAppearance()
{
	setEdgeType(EDGE_NONE);
	al::Color col;
	switch(state) {
		case State::DEFAULT: col = al::DarkGray; break;
		case State::HOVER: col = al::Blue; break;
		case State::DOWN: col = al::Black; break;
	}

	setBgColor(col);

	needsRedraw = true;
}

gui::DropdownList::ElementListWin::ElementListWin(DropdownList& parent)
	: Window(parent.listSize, {0, parent.getHeight()}),
	  slider({8, getHeight()}, {0,0}, 0),
	  elementHeight(18.f)
{
	visible = false;
	setBgColor(al::RGB(90,90,90));
	slider.setAlignment(ALIGN_RIGHT_CENTER);

	addChild(slider);

	registerEventHandler(ALLEGRO_EVENT_MOUSE_AXES, &ElementListWin::onMouseAxes);
}

void gui::DropdownList::ElementListWin::onMouseAxes(const ALLEGRO_EVENT &ev)
{
	for(auto& el: elements) {
		el->handleEvent(ev);
	}
	slider.handleEvent(ev);
	slider.onScroll(ev);
}

void gui::DropdownList::ElementListWin::tick()
{
	Window::tick();

	slider.setQuantization(std::max<uint32_t>(1,(int)elements.size()-1));

	for(auto& el: elements) {
		el->visible = false;
	}

	for(auto idx = slider.getRawValue(); idx<elements.size(); idx++) {
		auto offset = idx - slider.getRawValue();
		float uOffset = (elementHeight+2) * offset;
		if(uOffset > getHeight()) {
			break;
		}

		elements[idx]->visible = true;
		elements[idx]->setPos({0, uOffset});
		elements[idx]->resize({getWidth() - slider.getWidth(), elements[idx]->getHeight()});
	}
}



void gui::DropdownList::ElementListWin::render()
{
	Window::render();
}

