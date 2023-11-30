#pragma once

#include "Core/Resource.h"
#include "Gui/Widget.h"
#include "VFS/Serializer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Trinity
{
	class GuiSerializer;
	class ResourceCache;
	class Widget;
	class WidgetFactory;
	class WidgetEvent;
	class Panel;

	class Gui : public Resource
	{
	public:

		friend class GuiSerializer;

		Gui();
		virtual ~Gui() = default;

		Gui(const Gui&) = delete;
		Gui& operator = (const Gui&) = delete;

		Gui(Gui&&) = default;
		Gui& operator = (Gui&&) = default;

		Widget* getRoot() const
		{
			return mRoot;
		}

		WidgetFactory* getWidgetFactory() const
		{
			return mWidgetFactory.get();
		}

		std::vector<Widget*> getWidgets() const
		{
			std::vector<Widget*> result(mWidgets.size());
			std::transform(mWidgets.begin(), mWidgets.end(), result.begin(),
				[](auto& widget) {
					return widget.get();
				}
			);

			return result;
		}

		virtual std::type_index getType() const override;
		virtual Widget* findWidget(const std::string& name) const;
		virtual Widget* findWidget(const UUIDv4::UUID& uuid) const;

		virtual ISerializer* getSerializer();

		virtual void setRoot(Widget& widget);
		virtual void addChild(Widget& widget);

		virtual void addWidget(std::unique_ptr<Widget> widget);
		virtual void setWidgets(std::vector<std::unique_ptr<Widget>> widgets);
		virtual void updateWidgetUUID(Widget* widget, const UUIDv4::UUID& newUUID);

		virtual void layoutWidgets();
		virtual void registerDefaultWidgets();
		virtual void clear();
		 
		virtual Panel* addPanel(const glm::vec2& size, Widget* parent = nullptr);

	protected:

		Widget* mRoot{ nullptr };
		std::unique_ptr<WidgetFactory> mWidgetFactory{ nullptr };
		std::vector<std::unique_ptr<Widget>> mWidgets;
		std::unordered_map<UUIDv4::UUID, Widget*> mWidgetMap;
	};

	class GuiSerializer : public ISerializer
	{
	public:

		GuiSerializer() = default;
		virtual ~GuiSerializer() = default;

		GuiSerializer(const GuiSerializer&) = delete;
		GuiSerializer& operator = (const GuiSerializer&) = delete;

		GuiSerializer(GuiSerializer&&) = default;
		GuiSerializer& operator = (GuiSerializer&&) = default;

		virtual void setGui(Gui& gui);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Gui* mGui{ nullptr };
	};
}