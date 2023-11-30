#include "Gui/Gui.h"
#include "Gui/Widget.h"
#include "Gui/WidgetFactory.h"
#include "Gui/Widgets/Panel.h"
#include "VFS/FileSystem.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include <queue>

namespace Trinity
{	
	Gui::Gui()
	{
		mWidgetFactory = std::make_unique<WidgetFactory>();
		registerDefaultWidgets();
	}

	std::type_index Gui::getType() const
	{
		return typeid(Gui);
	}

	Widget* Gui::findWidget(const std::string& name) const
	{
		for (auto& widget : mWidgets)
		{
			if (widget->getName() == name)
			{
				return widget.get();
			}
		}

		return nullptr;
	}

	Widget* Gui::findWidget(const UUIDv4::UUID& uuid) const
	{
		if (auto it = mWidgetMap.find(uuid); it != mWidgetMap.end())
		{
			return it->second;
		}

		return nullptr;
	}

	ISerializer* Gui::getSerializer()
	{
		static GuiSerializer serializer;
		serializer.setGui(*this);

		return &serializer;
	}

	void Gui::setRoot(Widget& widget)
	{
		mRoot = &widget;
	}

	void Gui::addChild(Widget& widget)
	{
		mRoot->addChild(widget);
	}

	void Gui::addWidget(std::unique_ptr<Widget> widget)
	{
		mWidgetMap.insert(std::make_pair(widget->getUUID(), widget.get()));
		mWidgets.emplace_back(std::move(widget));
	}

	void Gui::setWidgets(std::vector<std::unique_ptr<Widget>> widgets)
	{
		for (auto& widget : widgets)
		{
			mWidgetMap.insert(std::make_pair(widget->getUUID(), widget.get()));
		}

		mWidgets = std::move(widgets);
	}

	void Gui::updateWidgetUUID(Widget* widget, const UUIDv4::UUID& newUUID)
	{
		if (auto it = mWidgetMap.find(widget->getUUID()); it != mWidgetMap.end())
		{
			mWidgetMap.erase(it);
		}

		widget->setUUID(newUUID);
		mWidgetMap.insert(std::make_pair(newUUID, widget));
	}

	void Gui::layoutWidgets()
	{
		if (mRoot != nullptr)
		{
			mRoot->layoutChildWidgets();
			mRoot->updateScreenPosition(glm::vec2{ 0.0f });
		}
	}

	void Gui::registerDefaultWidgets()
	{		
		mWidgetFactory->registerCreator<Panel>("Panel");
	}

	void Gui::clear()
	{
		mWidgets.clear();
		mRoot = nullptr;
	}

	Panel* Gui::addPanel(const glm::vec2& size, Widget* parent)
	{
		auto panel = std::make_unique<Panel>();
		panel->setName("Panel");
		panel->setSize(size);

		if (parent != nullptr)
		{
			parent->addChild(*panel);
		}
		else
		{
			addChild(*panel);
		}

		auto* panelPtr = panel.get();
		addWidget(std::move(panel));

		return panelPtr;
	}

	void GuiSerializer::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	bool GuiSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		uint32_t numWidgets{ 0 };
		if (!reader.read(&numWidgets))
		{
			LogError("FileReader::read() failed for Gui 'num widgets'");
			return false;
		}

		for (uint32_t idx = 0; idx < numWidgets; idx++)
		{
			auto uuid = UUIDv4::UUID::fromStrFactory(reader.readString());
			if (!mGui->mWidgetFactory->hasRegister(uuid))
			{
				LogError("Widget not register for uuid: '%s'", uuid.str().c_str());
				return false;
			}

			auto widget = mGui->mWidgetFactory->createWidget(uuid);
			if (widget != nullptr)
			{
				auto* widgetPtr = widget.get();
				mGui->addWidget(std::move(widget));
			
				if (auto* serializer = widgetPtr->getSerializer(*mGui); serializer != nullptr)
				{
					if (!serializer->read(reader, cache))
					{
						LogError("WidgetSerializer::read() failed for node: '%d'", idx);
						return false;
					}
				}
			}
		}

		auto rootUUID = UUIDv4::UUID::fromStrFactory(reader.readString());
		mGui->mRoot = mGui->findWidget(rootUUID);

		return true;
	}

	bool GuiSerializer::write(FileWriter& writer)
	{
		const uint32_t numWidgets = (uint32_t)mGui->mWidgets.size();
		if (!writer.write(&numWidgets))
		{
			LogError("FileWriter::write() failed for 'num widgets' for gui: '%s'", mGui->mName.c_str());
			return false;
		}

		for (auto& widget : mGui->mWidgets)
		{
			writer.writeString(widget->getTypeUUID().str());

			if (auto* serializer = widget->getSerializer(*mGui); serializer != nullptr)
			{
				if (!serializer->write(writer))
				{
					LogError("WidgetSerialzer::write() failed for widget: '%s'", widget->getName().c_str());
					return false;
				}
			}
		}

		std::string rootUUID;
		if (mGui->mRoot != nullptr)
		{
			rootUUID = mGui->mRoot->getUUID().str();
		}

		if (!writer.writeString(rootUUID))
		{
			LogError("FileWriter::write() failed for 'root uuid' for gui: '%s'", mGui->mName.c_str());
			return false;
		}

		return true;
	}

	bool GuiSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("widgets"))
		{
			LogError("Gui JSON object doesn't contain 'widgets' key");
			return false;
		}

		if (!object.contains("root"))
		{
			LogError("Gui JSON object doesn't contain 'root' key");
			return false;
		}

		for (auto widgetJson : object["widgets"])
		{
			if (!widgetJson.contains("uuid"))
			{
				LogError("Widget JSON object doesn't contain 'uuid' key");
				return false;
			}

			auto uuid = UUIDv4::UUID::fromStrFactory(widgetJson["uuid"].get<std::string>());
			if (!mGui->mWidgetFactory->hasRegister(uuid))
			{
				LogError("Widget not register for uuid: '%s'", uuid.str().c_str());
				return false;
			}

			auto widget = mGui->mWidgetFactory->createWidget(uuid);
			if (widget != nullptr)
			{
				auto* widgetPtr = widget.get();
				mGui->addWidget(std::move(widget));

				if (auto* serializer = widgetPtr->getSerializer(*mGui); serializer != nullptr)
				{
					if (!serializer->read(widgetJson, cache))
					{
						LogError("WidgetSerializer::read() failed for uuid: '%s'", uuid.str().c_str());
						return false;
					}
				}
			}
		}

		auto rootUUID = UUIDv4::UUID::fromStrFactory(object["root"].get<std::string>());
		mGui->mRoot = mGui->findWidget(rootUUID);

		return true;
	}

	bool GuiSerializer::write(json& object)
	{
		std::vector<json> widgetsJson;

		for (auto& widget : mGui->mWidgets)
		{
			if (auto* serializer = widget->getSerializer(*mGui); serializer != nullptr)
			{
				json widgetJson;
				if (!serializer->write(widgetJson))
				{
					LogError("ComponentSerialzer::write() failed for component: '%s'", widget->getName().c_str());
					return false;
				}

				widgetsJson.push_back(std::move(widgetJson));
			}
		}

		std::string rootUUID;
		if (mGui->mRoot != nullptr)
		{
			rootUUID = mGui->mRoot->getUUID().str();
		}

		object["widgets"] = std::move(widgetsJson);
		object["root"] = rootUUID;

		return true;
	}
}
