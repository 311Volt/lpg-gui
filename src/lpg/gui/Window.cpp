#include "lpg/gui/TitleBar.hpp"
#include <lpg/gui/Window.hpp>

#include <algorithm>
#include <fmt/format.h>

#include <axxegro/Transform.hpp>
#include <axxegro/event/EventQueue.hpp>
#include <axxegro/event/EventDispatcher.hpp>
#include <lpg/util/Log.hpp>

lpg::IntegerMap<uint32_t, gui::Window*> gui::Window::idMap;
lpg::ResourceManager gui::Window::RM;
float gui::Window::EnvScale = 1.0;

gui::Window::Window(al::Vec2<> size, al::Coord<> pos, Alignment align, EdgeType edge)
{
	resize(size);
	if(pos == POS_AUTO) {
		setPos({5,5});
	} else {
		setPos(pos);
	}

	textColor = al::RGB(0,0,0);
	bgColor = al::RGB(192,192,192);
	edgeType = edge;
	alignment = align;

	id = idMap.insert(this);

	title = fmt::format("Unnamed {} [#{}]", className(), id);
	creationTime = al::GetTime();

	zIndex = 0;
	focused = true;
	exitFlag = false;

	visible = true;

	needsRedraw = true;
	isPrerenderingEnabled = false;

	parent = nullptr;

	drawTime = 0.00001;

	eventDispatcher.setDefaultHandler([this](const ALLEGRO_EVENT& ev){
		for(const auto& childId: children) {
			Window* win = GetWindowByID(childId);
			if(!win) {
				continue;
			}
			GetWindowByID(childId)->handleEvent(ev);
		}
	});
}
gui::Window::~Window()
{
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

	//fmt::print("{} {} {} {}: {:06X}\n", win.a.x, win.a.y, win.b.x, win.b.y, bgColor.rgb_u32());

	if(edgeType == EDGE_REGULAR) {
		al::DrawRectangle(win, sh4);
	} else if(edgeType == EDGE_BEVELED) {
		al::DrawLine(win.bottomLeft(), win.bottomRight(), sh1);
		al::DrawLine(win.topRight(), win.bottomRight(), sh1);
		
		al::DrawLine(win.topLeft()-ob1x, win.topRight(), sh4);
		al::DrawLine(win.topLeft(), win.bottomLeft(), sh4);

		al::DrawLine(inner.bottomLeft(), inner.bottomRight(), sh2);
		al::DrawLine(inner.topRight(), inner.bottomRight(), sh2);
	} else if(edgeType == EDGE_BEVELED_INWARD) {
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

void gui::Window::registerEventHandlerProc(ALLEGRO_EVENT_TYPE evType, EventHandler handler)
{
	eventDispatcher.setEventTypeHandler(evType, handler);
}

void gui::Window::deleteEventHandler(ALLEGRO_EVENT_TYPE evType)
{
	eventDispatcher.deleteEventTypeHandler(evType);
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

void gui::Window::onResize()
{

}

void gui::Window::drawChildren()
{
	std::vector<uint32_t> ids;
	for(auto& id: children)
		ids.push_back(id);

	//sort by highest z-index, then highest id
	std::sort(ids.begin(), ids.end(), [&](uint32_t l, uint32_t r){
		return idMap[l]->isDrawnBefore(*idMap[r]);
	});

	for(unsigned i=0; i<ids.size(); i++) {
		idMap[ids[i]]->draw();
	} 
}


void gui::Window::handleEvent(const ALLEGRO_EVENT& ev)
{
	eventDispatcher.dispatch(ev);
}

void gui::Window::addChild(Window& child)
{
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

void gui::Window::give(std::unique_ptr<Window> child)
{
	child->parent = this;
	addChild(*child.get());
	ownedChildren[child->getID()] = std::move(child);
}

bool gui::Window::isDrawnBefore(Window &other) const
{
	if(zIndex != other.zIndex)
		return zIndex > other.zIndex;
	return id > other.id;
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
	return getRelPos() + (parent ? (parent->getAbsPos()) : al::Coord<>(0, 0));
}

al::Rect<> gui::Window::getRect() const
{
	return al::Rect(getRelPos(), getRelPos()+dims);
}

float gui::Window::ToPixels(float x)
{
	return x * GetEnvScale();
}
float gui::Window::ToUnits(float x)
{
	return x / GetEnvScale();
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
	onResize();
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

al::Vec2<> gui::Window::getParentSize() const
{
	if(parent) {
		return parent->getSize();
	} else {
		return al::CurrentDisplay.size();
	}
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


void gui::Window::normalizeChildrenZIndices()
{
	int idx = 0;

	std::vector<uint32_t> sorted;
	for(auto& childId: children) {
		fmt::print("child of #{}: #{}\n", id, childId);
		sorted.push_back(childId);
	}

	std::sort(sorted.begin(), sorted.end(), [&](uint32_t l, uint32_t r){
		return idMap[l]->isDrawnBefore(*idMap[r]);
	});

	for(unsigned i=0; i<sorted.size(); i++) {
		auto cid = sorted[i];
		idMap[cid]->zIndex = idx++;
		fmt::print("{} #{}: zindex={}\n", idMap[cid]->className(), cid, idMap[cid]->zIndex);
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
