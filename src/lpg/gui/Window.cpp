#include <lpg/gui/Window.hpp>

#include <algorithm>
#include <allegro5/allegro_primitives.h>
#include <fmt/format.h>

#include <axxegro/event/EventQueue.hpp>
#include <lpg/util/Log.hpp>

#include <lpg/util/VectorImage.hpp>

lpg::IntegerMap<uint32_t, gui::Window*> gui::Window::idMap;
lpg::ResourceManager gui::Window::RM;
float gui::Window::EnvScale = 1.0;

gui::Window::Window(float width, float height, float x, float y)
{
	resize(width, height);
	setPos(Point(x, y));

	textColor = al_map_rgb(0,0,0);
	bgColor = al_map_rgb(192,192,192);
	edgeType = EdgeType::BEVELED;
	alignment = Alignment::LEFT_TOP;

	id = idMap.insert(this);

	title = fmt::format("Unnamed window [#{}]", id);
	creationTime = al_get_time();

	zIndex = 0;
	focused = true;
	exitFlag = false;

	needsRedraw = true;
	isPrerenderingEnabled = false;

	parent = nullptr;
}

gui::Window::Window(float width, float height)
	: Window(width, height, 0, 0)
{

}

gui::Window::~Window()
{
	idMap.erase(id);

	for(auto& evType: propagatedEvents) {
		deleteEventHandler(evType);
	}

	if(parent) {
		parent->children.erase(id);
	}
}

void gui::Window::tick()
{
	for(auto& child: children) {
		idMap[child]->tick();
	}
}

void gui::Window::render()
{
	float w, h;
	getScreenSize(&w, &h);
	w = int(w);
	h = int(h);

	al_draw_filled_rectangle(0, 0, w, h, bgColor);

	if(edgeType == EdgeType::REGULAR) {
		al_draw_rectangle(0, 0, w, h, al_map_rgb(255,255,255), 1.0);
	} else if(edgeType == EdgeType::BEVELED) {
		al_draw_line(0, 1, w-1, 0, al_map_rgb(255,255,255), 1.0); //top
		al_draw_line(1, 0, 0, h-1, al_map_rgb(255,255,255), 1.0); //left
		al_draw_line(0, h, w, h, al_map_rgb(0, 0, 0), 1.0);       //bottom
		al_draw_line(w, 0, w, h, al_map_rgb(0, 0, 0), 1.0);       //right

		al_draw_line(0+1, h-1, w-1, h-1, al_premul_rgba(0, 0, 0, 86), 1.0); //bottom
		al_draw_line(w-1, 0+1, w-1, h-1, al_premul_rgba(0, 0, 0, 86), 1.0); //right
	} else if(edgeType == EdgeType::BEVELED_INWARD) {
		al_draw_line(0, 1, w-1, 0, al_map_rgb(0,0,0), 1.0); //top
		al_draw_line(1, 0, 0, h-1, al_map_rgb(0,0,0), 1.0); //left
		al_draw_line(0, h, w, h, al_map_rgb(255, 255, 255), 1.0);       //bottom
		al_draw_line(w, 0, w, h, al_map_rgb(255, 255, 255), 1.0);       //right

		al_draw_line(0+1, 1+1, w-1, 0+1, al_premul_rgba(0, 0, 0, 86), 1.0); //top
		al_draw_line(1+1, 0+1, 0+1, h-1, al_premul_rgba(0, 0, 0, 86), 1.0); //left
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
	Point rp = ToPixels(getAbsPos());

	ALLEGRO_TRANSFORM tr;
	al_identity_transform(&tr);
	al_translate_transform(&tr, int(rp.x), int(rp.y));
	al::ScopedTransform st(&tr);

	if(!isPrerenderingEnabled) {
		render();
	} else {

		if(needsRedraw) {
			al::Log(3, fmt::format("prerendering window #{1:} ({2:}) to bmp {0:p}",(void*)winFrameBuffer->ptr(), getID(), getTitle()).c_str());
			al::ScopedTargetBitmap tb(winFrameBuffer->ptr());
			al_clear_to_color(al_map_rgba(0,0,0,0));
			//al_draw_bitmap(al_get_backbuffer(al_get_current_display()), -rp.x, -rp.y, 0);
			render();
			needsRedraw = false;
		}

                /* al_draw_filled_rectangle(0, 0,
			al_get_bitmap_width(winFrameBuffer->ptr()),
			al_get_bitmap_height(winFrameBuffer->ptr()),
			al_premul_rgba(0,255,0,70)); */

		al_draw_bitmap(winFrameBuffer->ptr(), 0.0, 0.0, 0);
	}
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
		if(propagatedEvents.count(ev.type)) {
			GetWindowByID(childID)->handleEvent(ev);
		}
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


gui::Point gui::Window::getRelPos() const
{
	float xFactor = float(static_cast<uint8_t>(alignment) >> 2) * 0.5;
	float yFactor = float(static_cast<uint8_t>(alignment) & 3) * 0.5;
	Point ret = offset;

	if(parent) {
		ret.x += xFactor * (parent->getWidth() - getWidth());
		ret.y += yFactor * (parent->getHeight() - getHeight());
	}
	return ret;
}

gui::Point gui::Window::getAbsPos() const
{
	return getRelPos() + (parent ? (parent->getAbsPos()) : Point());
}

gui::Rect gui::Window::getRect() const
{
	return Rect(getRelPos(), getRelPos()+dims);
}

gui::Point gui::Window::ToPixels(gui::Point p)
{
	return p * GetEnvScale();
}

gui::Point gui::Window::ToUnits(gui::Point p)
{
	return p / GetEnvScale();
}

gui::Rect gui::Window::ToPixels(gui::Rect r)
{
	return Rect(ToPixels(r.a), ToPixels(r.b));
}

gui::Rect gui::Window::ToUnits(gui::Rect r)
{
	return Rect(ToUnits(r.a), ToUnits(r.b));
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

void gui::Window::resize(Point newDims)
{
	bool changed = dims!=newDims;
	dims = newDims;

	if(winFrameBuffer && winFrameBuffer->ptr() && changed) {
		//TODO speed this up?
		Point newss = getScreenSize();
		winFrameBuffer = std::make_unique<al::Bitmap>(newss.x, newss.y);
		needsRedraw = true;
	}
}

void gui::Window::resize(float w, float h)
{
	resize(Point(w,h));
}

gui::Rect gui::Window::getScreenRectangle() const
{
	return ToPixels(Rect(getAbsPos(), getAbsPos()+dims));
}

void gui::Window::getScreenRectangle(float* x1, float* y1, float* x2, float* y2) const
{
	Rect r = getScreenRectangle();
	*x1 = r.a.x;
	*y1 = r.a.y;
	*x2 = r.b.x;
	*y2 = r.b.y;
}

gui::Point gui::Window::getScreenSize() const
{
	Rect r = getScreenRectangle();
	return Point(r.getWidth(), r.getHeight());
}

void gui::Window::getScreenSize(float* w, float* h) const
{
	Point p = getScreenSize();
	*w = p.x;
	*h = p.y;
}

void gui::Window::setPos(Point p)
{
	offset = p;
}

void gui::Window::setAlignment(Alignment alignment)
{
	this->alignment = alignment;
}


void gui::Window::setTextColor(ALLEGRO_COLOR color)
{
	textColor = color;
}

void gui::Window::setBgColor(ALLEGRO_COLOR color)
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
		Point ss = getScreenSize();
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
			al::Log(1, fmt::format(
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
		al::Log(2, fmt::format(
			"new EnvScale ({.2f}) outside of acceptable range (.01-100), keeping it at {.2f}\n",
			envScale, EnvScale
		));
		return;
	}
	EnvScale = envScale;

	for(uint32_t i=0; i<idMap.size(); i++) {
		if(RM.idMap.contains(i)) {
			auto& res = RM.idMap[i];
			if(auto* r = dynamic_cast<lpg::VectorImage*>(res.get())) {
				r->rescale(EnvScale);
			}
			if(auto* r = dynamic_cast<al::Font*>(res.get())) {
				r->rescale(EnvScale);
			}
		}
	}

	for(uint32_t i=0; i<idMap.size(); i++) {
		if(idMap.contains(i)) {
			idMap[i]->onRescale();
		}
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
	std::vector<uint32_t> ret;
	for(uint32_t i=0; i<idMap.size(); i++) {
		if(idMap.contains(i)) {
			ret.push_back(i);
		}
	}
	return ret;
}
