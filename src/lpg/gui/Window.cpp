#include <lpg/gui/Window.hpp>

#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <fmt/format.h>

#include <axxegro/Transform.hpp>
#include <axxegro/event/EventQueue.hpp>
#include <lpg/util/Log.hpp>

lpg::IntegerMap<uint32_t, gui::Window*> gui::Window::idMap;
lpg::ResourceManager gui::Window::RM;
float gui::Window::EnvScale = 1.0;

gui::Window::Window(float width, float height, float x, float y)
{
	resize(width, height);
	setPos({x, y});

	textColor = al::RGB(0,0,0);
	bgColor = al::RGB(192,192,192);
	edgeType = EdgeType::BEVELED;
	alignment = Alignment::LEFT_TOP;

	id = idMap.insert(this);

	title = fmt::format("Unnamed window [#{}]", id);
	creationTime = al::GetTime();

	zIndex = 0;
	focused = true;
	exitFlag = false;

	visible = true;

	needsRedraw = true;
	isPrerenderingEnabled = false;

	parent = nullptr;

	drawTime = 0.00001;
}

gui::Window::Window(float width, float height)
	: Window(width, height, 0, 0)
{

}

gui::Window::~Window()
{
	for(auto& evType: propagatedEvents) {
		deleteEventHandler(evType);
	}
	
	if(parent) {
		parent->children.erase(id);
	}
	for(uint32_t childId: children) {
		idMap[childId]->parent = nullptr;
	}
	idMap.erase(id);
}

void gui::Window::tick()
{
	for(auto& child: children) {
		idMap[child]->tick();
	}
}

void gui::Window::render()
{
	al::Rect<int> win = getRelScreenRectangle();
	al::Vec2<float> ob1x = {1.0, 0};
	al::Rect<int> inner = {win.a + al::Vec2<int>(1,1), win.b - al::Vec2<int>(1,1)};

	al::Color sh1 = al::Black;
	al::Color sh2 = al::PremulRGBA(0,0,0,86);
	al::Color sh3 = bgColor;
	al::Color sh4 = al::White;

	al::DrawFilledRectangle(win, bgColor);
	if(edgeType == EdgeType::REGULAR) {
		al::DrawRectangle(win, sh4);
	} else if(edgeType == EdgeType::BEVELED) {

		al::DrawLine(win.bottomLeft(), win.bottomRight(), sh1);
		al::DrawLine(win.topRight(), win.bottomRight(), sh1);
		
		al::DrawLine(win.topLeft()-ob1x, win.topRight(), sh4);
		al::DrawLine(win.topLeft(), win.bottomLeft(), sh4);

		al::DrawLine(inner.bottomLeft(), inner.bottomRight(), sh2);
		al::DrawLine(inner.topRight(), inner.bottomRight(), sh2);
	} else if(edgeType == EdgeType::BEVELED_INWARD) {
		al::DrawLine(win.bottomLeft(), win.bottomRight(), sh4);
		al::DrawLine(win.topRight(), win.bottomRight(), sh4);
		
		al::DrawLine(win.topLeft(), win.bottomLeft(), sh2);
		al::DrawLine(win.topLeft(), win.topRight(), sh2);

		al::DrawLine(inner.topLeft()-ob1x, inner.topRight(), sh1);
		al::DrawLine(inner.topLeft(), inner.bottomLeft(), sh1);

		al::DrawLine(inner.bottomLeft(), inner.bottomRight(), sh3);
		al::DrawLine(inner.topRight(), inner.bottomRight(), sh3);
	}

	drawChildren();
}

void gui::Window::beginRender()
{

}

void gui::Window::endRender()
{

}

void gui::Window::draw()
{
	al::Coord<int> rp = ToPixels(getAbsPos());
	double tic = al::GetTime();
	al::Transform tr;
	tr.translate(rp);
	al::ScopedTransform st(tr);

	if(!visible)
		return;

	if(!isPrerenderingEnabled) {
		render();
	} else {
		if(needsRedraw) {
			lpg::Log(3, fmt::format("prerendering window #{1:} ({2:}) to bmp {0:p}",(void*)winFrameBuffer->ptr(), getID(), getTitle()));
			al::ScopedTargetBitmap tb(*winFrameBuffer);
			al::TargetBitmap.clearToColor(al::RGBA(0,0,0,0));
			render();
			needsRedraw = false;
		}

		winFrameBuffer->draw({0, 0});

	}
	drawTime = al::GetTime() - tic;
}

void gui::Window::registerEventHandlerProc(ALLEGRO_EVENT_TYPE type, gui::Window::EventHandler fn)
{
	if(eventHandlers.find(type) != eventHandlers.end()) {
		deleteEventHandler(type);
	}

	eventHandlers[type] = fn;
	if(parent) {
		parent->acknowledgeChildEventHandlerRegistration(type);
	}
}

void gui::Window::deleteEventHandler(ALLEGRO_EVENT_TYPE type)
{
	if(eventHandlers.find(type) == eventHandlers.end()) {
		return;
	}

	eventHandlers.erase(type);
	if(parent) {
		parent->acknowledgeChildEventHandlerDeletion(type);
	}
}


void gui::Window::onInput(ALLEGRO_EVENT* ev)
{
	propagateEvent(ev);
}

void gui::Window::onAdoption()
{

}

void gui::Window::onRescale()
{
	needsRedraw = true;
	resize(dims); //TODO replace this ugly hack... sometime in the far future
}

void gui::Window::onTitleChange()
{

}


void gui::Window::drawChildren()
{
	std::vector<uint32_t> ids;
	for(auto& id: children)
		ids.push_back(id);

	//sort by highest z-index, then highest id
	std::sort(ids.begin(), ids.end(), [&](uint32_t l, uint32_t r){
		int zl = idMap[l]->getZIndex();
		int zr = idMap[r]->getZIndex();
		if(zl != zr)
			return zl > zr;
		return l > r;
	});

	for(unsigned i=0; i<ids.size(); i++) {
		idMap[ids[i]]->draw();
	} 
}


void gui::Window::handleEvent(const ALLEGRO_EVENT& ev)
{
	if(eventHandlers.count(ev.type)) {
		eventHandlers[ev.type](ev);
	}
	for(auto& childID: children) {
		//TODO !!!FIX THIS AND ENABLE THIS AGAIN!!!
		//if(propagatedEvents.count(ev.type)) {
			GetWindowByID(childID)->handleEvent(ev);
		//}
	}
}

void gui::Window::propagateEvent(ALLEGRO_EVENT* ev)
{
	for(auto& child: children) {
		Window* ch = idMap[child];
		if(ch->isFocused()) {
			ch->onInput(ev);
		}
	}
}

void gui::Window::addChild(Window& child)
{
	for(auto& [type, handler]: child.eventHandlers) {
		acknowledgeChildEventHandlerRegistration(type);
	}

	child.parent = this;
	children.insert(child.getID());
	child.onAdoption();
}

void gui::Window::addChild(uint32_t id)
{
	addChild(*(idMap.at(id)));
}

void gui::Window::removeChild(Window& child)
{
	for(auto& [type, handler]: child.eventHandlers) {
		acknowledgeChildEventHandlerDeletion(type);
	}
	child.parent = nullptr;
	auto it = children.find(child.getID());
	if(it == children.end()) {
		throw std::runtime_error("Window::removeChild() called on a non-child node");
	}
	children.erase(it);
}

void gui::Window::printDrawTimeSummary()
{
	std::vector<uint32_t> ids = idMap.keys();
	
	std::sort(ids.begin(), ids.end(), [&](uint32_t lhs, uint32_t rhs){
		return idMap[lhs]->drawTime > idMap[rhs]->drawTime;
	});

	fmt::print("draw time of every window in the last frame: \n");
	for(auto& id: ids) {
		Window* win = idMap[id];
		std::string ts = "not measurable";
		if(win->drawTime != 0.0) {
			ts = fmt::format("{:.2f} us ({:.2f} Hz)", win->drawTime*1000000.0, 1.0/win->drawTime);
		}
		fmt::print("  [#{}] -> {} \"{}\": {}\n", id, win->className(), win->getTitle(), ts);
	}
	
	fmt::print("\n");
}

void gui::Window::removeChild(uint32_t id)
{
	removeChild(*idMap.at(id));
}

void gui::Window::give(std::unique_ptr<Window>&& child)
{
	child->parent = this;
	addChild(*child.get());
	ownedChildren[child->getID()] = std::move(child);
}


void gui::Window::setZIndex(int n)
{
	zIndex = n;
}

int gui::Window::getZIndex() const
{
	return zIndex;
}


al::Coord<> gui::Window::getRelPos() const
{
	float xFactor = float(static_cast<uint8_t>(alignment) >> 2) * 0.5;
	float yFactor = float(static_cast<uint8_t>(alignment) & 3) * 0.5;
	al::Coord<> ret = offset;

	if(parent) {
		ret.x += xFactor * (parent->getWidth() - getWidth());
		ret.y += yFactor * (parent->getHeight() - getHeight());
	}
	return ret;
}

al::Coord<> gui::Window::getAbsPos() const
{
	return getRelPos() + (parent ? (parent->getAbsPos()) : al::Coord<>());
}

al::Rect<> gui::Window::getRect() const
{
	return al::Rect(getRelPos(), getRelPos()+dims);
}

al::Coord<> gui::Window::ToPixels(const al::Coord<>& p)
{
	return p * GetEnvScale();
}

al::Coord<> gui::Window::ToUnits(const al::Coord<>& p)
{
	return p / GetEnvScale();
}

al::Rect<> gui::Window::ToPixels(const al::Rect<>& r)
{
	return al::Rect<>(ToPixels(r.a), ToPixels(r.b));
}

al::Rect<> gui::Window::ToUnits(const al::Rect<>& r)
{
	return al::Rect<>(ToUnits(r.a), ToUnits(r.b));
}


float gui::Window::getRelX() const
{
	return getRelPos().x;
}

float gui::Window::getRelY() const
{
	return getRelPos().y;
}

float gui::Window::getAbsX() const
{
	return getAbsPos().x;
}

float gui::Window::getAbsY() const
{
	return getAbsPos().y;
}

float gui::Window::getWidth() const
{
	return dims.x;
}

float gui::Window::getHeight() const
{
	return dims.y;
}

void gui::Window::resize(al::Vec2<> newDims)
{
	if(newDims.x < 0 || newDims.y < 0) {
		throw std::runtime_error(fmt::format("cannot resize window to {}x{}", newDims.x, newDims.y));
	}
	dims = newDims;
	if(winFrameBuffer && winFrameBuffer->size() != getScreenRectangle().size()) {
		al::Vec2<> newss = getScreenSize();
		winFrameBuffer = std::make_unique<al::Bitmap>(newss.x, newss.y);
		needsRedraw = true;
	}
}

void gui::Window::resize(float w, float h)
{
	resize(al::Vec2<>(w,h));
}

al::Rect<> gui::Window::getRelScreenRectangle() const
{
	return ToPixels(al::Rect<>::PosSize({}, dims));
}

al::Rect<> gui::Window::getScreenRectangle() const
{
	return ToPixels(al::Rect<>::PosSize(getAbsPos(), dims));
}



void gui::Window::getScreenRectangle(float* x1, float* y1, float* x2, float* y2) const
{
	auto r = getScreenRectangle();
	*x1 = r.a.x;
	*y1 = r.a.y;
	*x2 = r.b.x;
	*y2 = r.b.y;
}

al::Vec2<> gui::Window::getScreenSize() const
{
	auto r = getScreenRectangle();
	return {r.width(), r.height()};
}

al::Vec2<> gui::Window::getSize() const
{
	return dims;
}

void gui::Window::getScreenSize(float* w, float* h) const
{
	auto p = getScreenSize();
	*w = p.x;
	*h = p.y;
}

void gui::Window::setPos(const al::Coord<>& p)
{
	offset = p;
}

void gui::Window::setAlignment(Alignment alignment)
{
	this->alignment = alignment;
}


void gui::Window::setTextColor(al::Color color)
{
	textColor = color;
}

void gui::Window::setBgColor(al::Color color)
{
	bgColor = color;
}

void gui::Window::setEdgeType(EdgeType type)
{
	edgeType = type;
}

void gui::Window::setTitle(const std::string& title)
{
	bool changed = this->title != title;
	if(changed) {
		this->title = title;
		onTitleChange();
		needsRedraw = true;
	}
}

void gui::Window::enablePrerendering()
{
	if(!isPrerenderingEnabled) {
		isPrerenderingEnabled = true;
		auto ss = getScreenSize();
		winFrameBuffer = std::make_unique<al::Bitmap>(ss.x, ss.y);
		needsRedraw = true;
	}
}

void gui::Window::disablePrerendering()
{
	if(isPrerenderingEnabled) {
		isPrerenderingEnabled = false;
		winFrameBuffer = std::unique_ptr<al::Bitmap>();
	}
}


void gui::Window::setExitFlag()
{
	exitFlag = true;
}

const std::string& gui::Window::getTitle() const
{
	return title;
}

uint32_t gui::Window::getID() const
{
	return id;
}

bool gui::Window::isFocused() const
{
	return focused;
}

void gui::Window::acknowledgeChildEventHandlerRegistration(ALLEGRO_EVENT_TYPE type)
{
	for(Window* it=this; it; it=it->parent) {
		it->propagatedEvents.insert(type);
	}
}

void gui::Window::acknowledgeChildEventHandlerDeletion(ALLEGRO_EVENT_TYPE type)
{
	for(Window* it=this; it; it=it->parent) {
		auto& pEv = it->propagatedEvents;
		auto item = pEv.find(type);
		if(item != pEv.end()) {
			pEv.erase(item);
		} else {
			lpg::Log(1, fmt::format(
				"warning: event {} handled by \"{}\" (#{}) not found in propagatedEvents of parent \"{}\" (#{}) (this should never happen)",
				type,
				getTitle(),
				getID(),
				it->getTitle(),
				it->getID()
			));
		}
	}
}


void gui::Window::normalizeChildrenZIndices()
{
	int idx = 0;

	std::vector<uint32_t> sorted;
	for(auto& child: children) {
		sorted.push_back(idMap[child]->getID());
	}

	std::sort(sorted.begin(), sorted.end(), [&](uint32_t lhs, uint32_t rhs){
		Window* l = idMap[lhs];
		Window* r = idMap[rhs];
		if(l->zIndex == r->zIndex)
			return l->getID() < r->getID();
		return l->zIndex < r->zIndex;
	});

	for(unsigned i=0; i<sorted.size(); i++) {
		if(i && (idMap[i]->zIndex > idMap[i-1]->zIndex)) {
			idx++;
		}
		idMap[i]->zIndex = idx;
	}
}

float gui::Window::GetEnvScale()
{
	return EnvScale;
}

void gui::Window::SetEnvScale(float envScale)
{
	if(envScale <= 0.01f || envScale > 100.0f) {
		lpg::Log(2, fmt::format(
			"new EnvScale ({.2f}) outside of acceptable range (.01-100), keeping it at {.2f}\n",
			envScale, EnvScale
		));
		return;
	}
	EnvScale = envScale;

	RM.setScale({envScale, envScale});

	for(auto [id, win]: idMap) {
		win->onRescale();
	}
}



gui::Window* gui::Window::GetWindowByID(uint32_t id)
{
	if(idMap.contains(id))
		return idMap[id];
	return nullptr;
}

std::vector<uint32_t> gui::Window::GetAllWindowIDs()
{
	return idMap.keys();
}
