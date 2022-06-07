#ifndef INCLUDE_LPG_GUI_WINDOW
#define INCLUDE_LPG_GUI_WINDOW

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

	constexpr al::Coord<float> POS_AUTO = {-1, -1};

	class Window {
	public:

	
		enum EdgeType {
			EDGE_NONE = 0,
			EDGE_REGULAR = 1,
			EDGE_BEVELED = 2,
			EDGE_BEVELED_INWARD = 3
		};
		enum Alignment {
			/* upper 2 bits: 00=left, 01=center, 10=right
			 * lower 2 bits: 00=top, 01=center, 10=bottom */
			ALIGN_LEFT_TOP      = 0b0000,
			ALIGN_LEFT_CENTER   = 0b0001,
			ALIGN_LEFT_BOTTOM   = 0b0010,
			ALIGN_CENTER_TOP    = 0b0100,
			ALIGN_CENTER        = 0b0101,
			ALIGN_CENTER_BOTTOM = 0b0110,
			ALIGN_RIGHT_TOP     = 0b1000,
			ALIGN_RIGHT_CENTER  = 0b1001,
			ALIGN_RIGHT_BOTTOM  = 0b1010
		};

		Window(
			al::Vec2<> size, 
			al::Coord<> pos = POS_AUTO, 
			Alignment align = ALIGN_LEFT_TOP,
			EdgeType edge = EDGE_BEVELED
		);

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&&) = delete;
		Window& operator=(Window&&) = delete;
		
		virtual ~Window();

		virtual std::string_view className() const {return "Window";};
		#define LPG_WIN_CLS_NAME(name) virtual std::string_view className() const override {return name;}

		virtual void tick();
		virtual void render();
		virtual void onAdoption();
		virtual void onRescale();
		virtual void onTitleChange();
		virtual void onResize();

		void beginRender();
		void endRender();

		using EventHandler = std::function<void(const ALLEGRO_EVENT&)>;


		void registerEventHandlerProc(ALLEGRO_EVENT_TYPE evType, EventHandler handler);
		void deleteEventHandler(ALLEGRO_EVENT_TYPE evType);

		template<typename T>
		using EventHandlerMember = void(T::*)(const ALLEGRO_EVENT&);

		template<typename T>
		void registerEventHandler(ALLEGRO_EVENT_TYPE evType, EventHandlerMember<T> handler)
		{
			registerEventHandlerProc(evType, [handler,this](const ALLEGRO_EVENT& ev){
				if(T* win = dynamic_cast<T*>(this)) {
					std::invoke(handler, win, ev);
				}
			});
		}

		void draw();

		void drawChildren();
		void handleEvent(const ALLEGRO_EVENT& ev);

		void addChild(Window& child);
		void addChild(uint32_t id);

		void removeChild(Window& child);
		void removeChild(uint32_t id);

		bool isDrawnBefore(Window& other) const;

		void give(std::unique_ptr<Window> child);

		void setZIndex(int n);
		int getZIndex() const;

		al::Coord<> getRelPos() const;
		al::Coord<> getAbsPos() const;
		al::Rect<> getRect() const;

		static float ToPixels(float x);
		static float ToUnits(float x);

		static al::Coord<> ToPixels(const al::Coord<>& p);
		static al::Coord<> ToUnits(const al::Coord<>& p);

		static al::Rect<> ToPixels(const al::Rect<>& r);
		static al::Rect<> ToUnits(const al::Rect<>& r);

		float getRelX() const;
		float getRelY() const;

		float getAbsX() const;
		float getAbsY() const;

		float getWidth() const;
		float getHeight() const;

		void resize(al::Coord<> newDims);
		void resize(float w, float h);

		void setAlignment(Alignment alignment);

		void getScreenRectangle(float* x1, float* y1, float* x2, float* y2) const;
		al::Rect<> getScreenRectangle() const;

		al::Rect<> getRelScreenRectangle() const;
		
		al::Vec2<> getSize() const;
		void getScreenSize(float* w, float* h) const;
		al::Vec2<> getScreenSize() const;

		al::Vec2<> getParentSize() const;

		void setPos(const al::Coord<>& p);

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
		
		bool visible;

	protected:
		static float EnvScale;

		//local management
		std::unordered_map<uint32_t, std::unique_ptr<Window>> ownedChildren;
		void normalizeChildrenZIndices();

		/**********************************************/

		al::EventDispatcher eventDispatcher;

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
		al::Coord<> offset;
		al::Coord<> dims;

		//metadata
		uint32_t id;
		std::string title;
		double creationTime;
		bool exitFlag;

		//window order
		int zIndex;
		bool focused;
	private:
		static lpg::IntegerMap<uint32_t, Window*> idMap;

		double drawTime;
	};
}

#endif /* INCLUDE_LPG_GUI_WINDOW */
