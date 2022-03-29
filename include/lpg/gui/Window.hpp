#ifndef LPG_GUI_WINDOW_H
#define LPG_GUI_WINDOW_H

#include <allegro5/allegro.h>

#include <cstdint>

#include <set>
#include <queue>
#include <memory>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <type_traits>

#include <axxegro/resources/Bitmap.hpp>
#include <axxegro/resources/Font.hpp>
#include <axxegro/math/math.hpp>

#include <lpg/gui/Mouse.hpp>

#include <lpg/resmgr/ResourceManager.hpp>
#include <lpg/util/IntegerMap.hpp>

namespace gui {

	using al::Point, al::Vec2, al::Rect;

	class Window {
	public:
		Window(float width, float height, float x, float y);
		Window(float width, float height);
		~Window();

		enum class EdgeType: uint8_t {
			NONE = 0,
			REGULAR = 1,
			BEVELED = 2,
			BEVELED_INWARD = 3
		};
		enum class Alignment: uint8_t {
			/* upper 2 bits: 00=left, 01=center, 10=right
			 * lower 2 bits: 00=top, 01=center, 10=bottom */
			LEFT_TOP      = 0b0000,
			LEFT_CENTER   = 0b0001,
			LEFT_BOTTOM   = 0b0010,
			CENTER_TOP    = 0b0100,
			CENTER        = 0b0101,
			CENTER_BOTTOM = 0b0110,
			RIGHT_TOP     = 0b1000,
			RIGHT_CENTER  = 0b1001,
			RIGHT         = 0b1010
		};
		using EventHandler = std::function<void(const ALLEGRO_EVENT&)>;


		virtual void tick();
		virtual void render();
		virtual void onInput(ALLEGRO_EVENT* ev);
		virtual void onAdoption();
		virtual void onRescale();
		virtual void onTitleChange();

		void beginRender();
		void endRender();

		void draw();

		void drawChildren();
		void handleEvent(const ALLEGRO_EVENT& ev);
		void propagateEvent(ALLEGRO_EVENT* ev);

		void registerEventHandlerProc(ALLEGRO_EVENT_TYPE type, EventHandler fn);
		void deleteEventHandler(ALLEGRO_EVENT_TYPE type);

		template<typename T>
		using EventHandlerMember = void(T::*)(const ALLEGRO_EVENT&);

		template<typename T>
		void registerEventHandler(ALLEGRO_EVENT_TYPE type, EventHandlerMember<T> fn)
		{
			static_assert(std::is_base_of<Window, T>::value);
			if(fn == nullptr) {
				throw std::runtime_error("nullptr given as event handler");
			}
			registerEventHandlerProc(type, [this,fn](const ALLEGRO_EVENT& ev)->void{
				T* p = static_cast<T*>(this);
				(p->*fn)(ev);
			});
		}


		void addChild(Window& child);
		void addChild(uint32_t id);

		void removeChild(Window& child);
		void removeChild(uint32_t id);

		void give(std::unique_ptr<Window>&& child);

		void setZIndex(int n);
		int getZIndex() const;

		Point getRelPos() const;
		Point getAbsPos() const;
		Rect getRect() const;

		static Point ToPixels(Point p);
		static Point ToUnits(Point p);

		static Rect ToPixels(Rect r);
		static Rect ToUnits(Rect r);

		float getRelX() const;
		float getRelY() const;

		float getAbsX() const;
		float getAbsY() const;

		float getWidth() const;
		float getHeight() const;

		void resize(Point newDims);
		void resize(float w, float h);

		void setAlignment(Alignment alignment);

		void getScreenRectangle(float* x1, float* y1, float* x2, float* y2) const;
		Rect getScreenRectangle() const;

		void getScreenSize(float* w, float* h) const;
		Point getScreenSize() const;

		void setPos(Point p);

		void setTextColor(al::Color color);
		void setBgColor(al::Color color);
		void setEdgeType(EdgeType type);

		void enablePrerendering();
		void disablePrerendering();

		static void printDrawTimeSummary();

		void setTitle(const std::string& title);
		const std::string& getTitle() const;
		uint32_t getID() const;

		bool isFocused() const;
		void setExitFlag();

		static float GetEnvScale();
		static void SetEnvScale(float envScale);

		static Window* GetWindowByID(uint32_t id);
		static std::vector<uint32_t> GetAllWindowIDs();

		static lpg::ResourceManager RM;

	protected:
		static float EnvScale;

		//local management
		std::unordered_map<uint32_t, std::unique_ptr<Window>> ownedChildren;
		std::unordered_map<ALLEGRO_EVENT_TYPE, EventHandler> eventHandlers;
		void normalizeChildrenZIndices();

		/**********************************************/

		//hierarchy
		std::set<uint32_t> children;
		Window* parent; //NULL for root

		//appearance
		al::Color textColor;
		al::Color bgColor;
		EdgeType edgeType;

		//rendering settings and effects
		std::unique_ptr<al::Bitmap> winFrameBuffer;
		bool isPrerenderingEnabled;
		bool needsRedraw;

		//position and dimensions
		Alignment alignment;
		Point offset;
		Point dims;

		//metadata
		uint32_t id;
		std::string title;
		double creationTime;
		bool exitFlag;

		//window order
		int zIndex;
		bool focused;
	private:
		void acknowledgeChildEventHandlerRegistration(ALLEGRO_EVENT_TYPE type);
		void acknowledgeChildEventHandlerDeletion(ALLEGRO_EVENT_TYPE type);

		std::unordered_multiset<ALLEGRO_EVENT_TYPE> propagatedEvents;
		static lpg::IntegerMap<uint32_t, Window*> idMap;

		double drawTime;
	};
}

#endif // LPG_GUI_WINDOW_H
