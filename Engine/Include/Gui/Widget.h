#pragma once

#include "Gui/WidgetTransform.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <string>
#include <memory>
#include <vector>
#include <typeindex>
#include "glm/glm.hpp"
#include "uuid_v4.h"

namespace Trinity
{
	class Gui;
	class GuiRenderer;
	class WidgetEvent;
	class WidgetSerializer;
	class WidgetEditor;

	enum class AlignVertical
	{
		DontCare,
		Top,
		Bottom,
		Center
	};

	enum class AlignHorizontal
	{
		DontCare,
		Left,
		Right,
		Center
	};

	enum FillMode
	{
		DontCare = (1 << 0),
		Horizontal = (1 << 1),
		Vertical = (1 << 2),
		Both = Horizontal | Vertical
	};

	class Widget
	{
	public:

		friend class WidgetEditor;
		friend class WidgetSerializer;

		Widget();
		virtual ~Widget() = default;

		Widget(const Widget&) = delete;
		Widget& operator = (const Widget&) = delete;

		Widget(Widget&&) = default;
		Widget& operator = (Widget&&) = default;

		const std::string& getName() const
		{
			return mName;
		}

		const UUIDv4::UUID& getUUID() const
		{
			return mUUID;
		}

		const glm::vec2& getPosition() const
		{
			return mPosition;
		}

		const glm::vec2& getScreenPosition() const
		{
			return mScreenPosition;
		}

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		const glm::vec4& getPadding() const
		{
			return mPadding;
		}

		const glm::vec4& getMargin() const
		{
			return mMargin;
		}

		WidgetTransform& getTransform()
		{
			return mTransform;
		}

		AlignVertical getAlignVertical() const
		{
			return mAlignVertical;
		}

		AlignHorizontal getAlignHorizontal() const
		{
			return mAlignHorizontal;
		}

		FillMode getFillMode() const
		{
			return mFillMode;
		}

		bool isEnabled() const
		{
			if (mParent != nullptr)
			{
				return mEnabled && mParent->isEnabled();
			}

			return mEnabled;
		}

		bool isVisible() const
		{
			if (mParent != nullptr)
			{
				return mVisible && mParent->isVisible();
			}

			return mVisible;
		}

		bool isSizeToContent() const
		{
			return mSizeToContent;
		}

		bool isMouseOver() const
		{
			return mMouseOver;
		}

		Widget* getParent() const
		{
			return mParent;
		}

		const std::vector<Widget*>& getChildren() const
		{
			return mChildren;
		}

		uint32_t getNumChildren() const
		{
			return (uint32_t)mChildren.size();
		}

		Widget* getChild(uint32_t index) const
		{
			if (index < (uint32_t)mChildren.size())
			{
				return mChildren.at(index);
			}

			return nullptr;
		}

		virtual std::type_index getType() const = 0;
		virtual UUIDv4::UUID getTypeUUID() const = 0;

		virtual IEditor* getEditor(Gui& gui);
		virtual ISerializer* getSerializer(Gui& gui);

		virtual void setName(const std::string& name);
		virtual void setUUID(const UUIDv4::UUID& uuid);
		virtual void setPosition(const glm::vec2& position);
		virtual void setSize(const glm::vec2& size);
		virtual void setPadding(const glm::vec4& padding);
		virtual void setMargin(const glm::vec4& margin);

		virtual void setAlignVertical(AlignVertical alignVertical);
		virtual void setAlignHorizontal(AlignHorizontal alignHorizontal);
		virtual void setFillMode(FillMode fillMode);
		virtual void setEnabled(bool enabled);
		virtual void setVisible(bool visible);
		virtual void setSizeToContent(bool sizeToContent);
		virtual void setMouseOver(bool mouseOver);

		virtual void setParent(Widget& parent);
		virtual void addChild(Widget& child);
		virtual void removeChild(Widget& child);
		virtual void layoutChildWidgets();
		virtual void updateScreenPosition(const glm::vec2& parentPos);

		virtual bool isPointOver(float x, float y);
		virtual bool handleEvent(const WidgetEvent& widgetEvt);
		virtual void update(float deltaTime);
		virtual void draw(GuiRenderer& renderer);

	protected:

		virtual void preUpdate(float deltaTime);
		virtual void postUpdate(float deltaTime);
		virtual void preDraw(GuiRenderer& renderer);
		virtual void postDraw(GuiRenderer& renderer);

	protected:

		std::string mName;
		UUIDv4::UUID mUUID;
		glm::vec2 mPosition{ 0.0f };
		glm::vec2 mScreenPosition{ 0.0f };
		glm::vec2 mSize{ 0.0f };
		glm::vec2 mScreenSize{ 0.0f };
		glm::vec4 mPadding{ 0.0f };
		glm::vec4 mMargin{ 0.0f };
		WidgetTransform mTransform;
		AlignVertical mAlignVertical{ AlignVertical::DontCare };
		AlignHorizontal mAlignHorizontal{ AlignHorizontal::DontCare };
		FillMode mFillMode{ FillMode::DontCare };
		bool mEnabled{ true };
		bool mVisible{ true };
		bool mSizeToContent{ false };
		bool mMouseOver{ false };
		Widget* mParent{ nullptr };
		std::vector<Widget*> mChildren;
	};

	class WidgetEditor : public IEditor
	{
	public:

		WidgetEditor() = default;
		virtual ~WidgetEditor() = default;

		WidgetEditor(const WidgetEditor&) = delete;
		WidgetEditor& operator = (const WidgetEditor&) = delete;

		WidgetEditor(WidgetEditor&&) = default;
		WidgetEditor& operator = (WidgetEditor&&) = default;

		virtual void setWidget(Widget& widget);
		virtual void setGui(Gui& gui);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		virtual void addCommonFields(const IEditorLayout& layout, ResourceCache& cache);
		virtual void addTransform(const IEditorLayout& layout, ResourceCache& cache);

		virtual const char* getAlignVerticalString(AlignVertical alignVertical) const;
		virtual const char* getAlignHorizontalString(AlignHorizontal alignHorizontal) const;
		virtual const char* getFillModeString(FillMode fillMode) const;

	protected:

		Widget* mWidget{ nullptr };
		Gui* mGui{ nullptr };
	};

	class WidgetSerializer : public ISerializer
	{
	public:

		WidgetSerializer() = default;
		virtual ~WidgetSerializer() = default;

		WidgetSerializer(const WidgetSerializer&) = delete;
		WidgetSerializer& operator = (const WidgetSerializer&) = delete;

		WidgetSerializer(WidgetSerializer&&) = default;
		WidgetSerializer& operator = (WidgetSerializer&&) = default;

		virtual void setWidget(Widget& widget);
		virtual void setGui(Gui& gui);

		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Widget* mWidget{ nullptr };
		Gui* mGui{ nullptr };
	};
}