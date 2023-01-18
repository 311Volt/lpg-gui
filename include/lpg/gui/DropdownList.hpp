#ifndef LPG_GUI_INCLUDE_LPG_GUI_DROPDOWNLIST
#define LPG_GUI_INCLUDE_LPG_GUI_DROPDOWNLIST

#include <lpg/gui/Window.hpp>
#include <lpg/gui/Text.hpp>
#include <lpg/gui/Slider.hpp>

namespace gui {
	class DropdownList: public Window {
	public:
		struct ElementData {
			uint32_t id;
			std::string text;
		};

		DropdownList(al::Vec2<> size, al::Coord<> pos, al::Vec2<> listSize);

		using OnChangeFn = std::function<void(uint32_t)>;

		void setOnChangeCallback(OnChangeFn fn);

		void append(const ElementData& dat);
		void setElements(const std::vector<ElementData>& data);

		const ElementData& getCurrentSelectionData();
		uint32_t getCurrentSelectionId();
		uint32_t getCurrentSelectionIndex();

		virtual void tick() override;
		virtual void render() override;

		bool changeSelection(uint32_t idx);

	private:
		void onMouseDown(const ALLEGRO_EVENT& ev);

		class Element: public Button {
		public:
			Element(al::Vec2<> size, ElementData dat, uint32_t idx, CallbackT callback);
			const ElementData& getData();

			virtual void onHover() override;
			std::function<void(uint32_t)> onHoverCallback;
			ElementData dat;
			uint32_t id;
			uint32_t idx;
		protected:
			virtual void updateAppearance() override;
		};

		class ElementListWin: public Window {
		public:
			ElementListWin(DropdownList& parent);
			std::vector<std::unique_ptr<Element>> elements;

			void onMouseAxes(const ALLEGRO_EVENT& ev);
			virtual void tick() override;
			virtual void render() override;
			Slider slider;
			float elementHeight;
		private:
		};

		al::Vec2<> listSize;
		ElementListWin elementListWin;
		Button downBtn;
		Text text;
		int vPos;
		int selectedIdx;
		int focusedIdx;


		OnChangeFn onChange;
	};
}

#endif /* LPG_GUI_INCLUDE_LPG_GUI_DROPDOWNLIST */
