#include "Gui/Widget.h"
#include "Gui/GuiRenderer.h"
#include "Gui/Gui.h"
#include "Editor/EditorLayout.h"
#include "Core/ResourceCache.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	Widget::Widget()
	{
		static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
		mUUID = uuidGenerator.getUUID();

		mTransform.setWidget(*this);
	}

	std::type_index Widget::getType() const
	{
		return typeid(Widget);
	}

	UUIDv4::UUID Widget::getTypeUUID() const
	{
		return mUUID;
	}

	IEditor* Widget::getEditor(Gui& gui)
	{
		return nullptr;
	}

	ISerializer* Widget::getSerializer(Gui& gui)
	{
		return nullptr;
	}

	void Widget::setName(const std::string& name)
	{
		mName = name;
	}

	void Widget::setUUID(const UUIDv4::UUID& uuid)
	{
		mUUID = uuid;
	}

	void Widget::setPosition(const glm::vec2& position)
	{
		mPosition = position;
	}

	void Widget::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void Widget::setPadding(const glm::vec4& padding)
	{
		mPadding = padding;
	}

	void Widget::setMargin(const glm::vec4& margin)
	{
		mMargin = margin;
	}

	void Widget::setAlignVertical(AlignVertical alignVertical)
	{
		mAlignVertical = alignVertical;
	}

	void Widget::setAlignHorizontal(AlignHorizontal alignHorizontal)
	{
		mAlignHorizontal = alignHorizontal;
	}

	void Widget::setFillMode(FillMode fillMode)
	{
		mFillMode = fillMode;
	}

	void Widget::setEnabled(bool enabled)
	{
		mEnabled = enabled;
	}

	void Widget::setVisible(bool visible)
	{
		mVisible = visible;
	}

	void Widget::setSizeToContent(bool sizeToContent)
	{
		mSizeToContent = sizeToContent;
	}

	void Widget::setMouseOver(bool mouseOver)
	{
		mMouseOver = mouseOver;
	}

	void Widget::setParent(Widget& parent)
	{
		mParent = &parent;
		mTransform.invalidateWorldMatrix();
	}

	void Widget::addChild(Widget& child)
	{
		child.setParent(*this);
		mChildren.push_back(&child);
	}

	void Widget::removeChild(Widget& child)
	{
		auto it = std::find(mChildren.begin(), mChildren.end(), &child);
		if (it != mChildren.end())
		{
			mChildren.erase(it);
		}
	}

	void Widget::layoutChildWidgets()
	{
		for (auto* child : mChildren)
		{
			child->layoutChildWidgets();
		}

		if (mSizeToContent)
		{
			glm::vec2 maxSize{ 0.0f };

			for (auto* child : mChildren)
			{
				auto& margin = child->mMargin;

				if (child->mScreenSize.x > maxSize.x)
				{
					maxSize.x = child->mScreenSize.x + margin.x + margin.z;
				}

				if (child->mScreenSize.y > maxSize.y)
				{
					maxSize.y = child->mScreenSize.y + margin.y + margin.w;
				}
			}

			mSize = maxSize;
		}

		mScreenSize = mSize + glm::vec2{ mPadding.x + mPadding.z, 
			mPadding.y + mPadding.w };

		for (auto* child : mChildren)
		{
			if (!mSizeToContent)
			{
				if ((child->mFillMode & FillMode::Vertical) != 0)
				{
					child->mSize.y = mSize.y - child->mPosition.y;
				}

				if ((child->mFillMode & FillMode::Horizontal) != 0)
				{
					child->mSize.x = mSize.x - child->mPosition.x;
				}

				child->mScreenSize = child->mSize + glm::vec2{ 
					child->mPadding.x + child->mPadding.z,
					child->mPadding.y + child->mPadding.w 
				};
			}

			child->mPosition = glm::vec2{ mPadding.x + child->mMargin.x, 
				mPadding.w + child->mMargin.w };

			switch (child->mAlignVertical)
			{
			case AlignVertical::Top:
				child->mPosition = glm::vec2{ child->mPosition.x, mScreenSize.y - (mPadding.y + child->mMargin.y + 
					child->mScreenSize.y) };
				break;

			case AlignVertical::Bottom:
				child->mPosition = glm::vec2{ child->mPosition.x, mPadding.w + child->mMargin.w };
				break;

			case AlignVertical::Center:
				child->mPosition = glm::vec2{ child->mPosition.x, (mScreenSize.y - (child->mScreenSize.y + 
					child->mMargin.y + child->mMargin.w)) / 2.0f };
				break;
			}

			switch (child->mAlignHorizontal)
			{
			case AlignHorizontal::Left:
				child->mPosition = glm::vec2{ mPadding.x + child->mMargin.x, child->mPosition.y };
				break;

			case AlignHorizontal::Right:
				child->mPosition = glm::vec2{ mScreenSize.x - mPadding.z - (child->mScreenSize.x + 
					child->mMargin.z), child->mPosition.y };
				break;

			case AlignHorizontal::Center:
				child->mPosition = glm::vec2{ (mScreenSize.x - (child->mScreenSize.x + child->mMargin.x + 
					child->mMargin.z)) / 2.0f, child->mPosition.y };
				break;
			}
		}
	}

	void Widget::updateScreenPosition(const glm::vec2& parentPos)
	{	
		mScreenPosition = parentPos + mPosition;

		for (auto* child : mChildren)
		{
			child->updateScreenPosition(mScreenPosition);
		}
	}

	bool Widget::isPointOver(float x, float y)
	{
		return (x >= mScreenPosition.x && x <= mScreenPosition.x + mScreenSize.x) &&
			(y >= mScreenPosition.y && y <= mScreenPosition.y + mScreenSize.y);
	}

	bool Widget::handleEvent(const WidgetEvent& widgetEvt)
	{
		return false;
	}

	void Widget::update(float deltaTime)
	{
		if (!mEnabled)
		{
			return;
		}

		preUpdate(deltaTime);

		for (auto* child : mChildren)
		{
			child->update(deltaTime);
		}

		postUpdate(deltaTime);
	}

	void Widget::draw(GuiRenderer& renderer)
	{
		if (!mVisible)
		{
			return;
		}

		preDraw(renderer);

		for (auto* child : mChildren)
		{
			child->draw(renderer);
		}

		postDraw(renderer);
	}

	void Widget::preUpdate(float deltaTime)
	{
	}

	void Widget::postUpdate(float deltaTime)
	{
	}

	void Widget::preDraw(GuiRenderer& renderer)
	{
	}

	void Widget::postDraw(GuiRenderer& renderer)
	{
	}

	void WidgetEditor::setWidget(Widget& widget)
	{
		mWidget = &widget;
	}

	void WidgetEditor::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	void WidgetEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{		
	}

	void WidgetEditor::addCommonFields(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Common"))
		{		
			if (layout.inputVec2("Size", mWidget->mSize))
			{
				mGui->layoutWidgets();
			}

			if (layout.inputVec4("Padding", mWidget->mPadding))
			{
				mGui->layoutWidgets();
			}

			if (layout.inputVec4("Margin", mWidget->mMargin))
			{
				mGui->layoutWidgets();
			}

			layout.checkbox("Enabled", mWidget->mEnabled);
			layout.checkbox("Visible", mWidget->mVisible);

			if (layout.beginCombo("Align Vertical", getAlignVerticalString(mWidget->mAlignVertical)))
			{
				if (layout.comboItem(getAlignVerticalString(AlignVertical::DontCare), 
					mWidget->mAlignVertical == AlignVertical::DontCare))
				{
					mWidget->mAlignVertical = AlignVertical::DontCare;
				}

				if (layout.comboItem(getAlignVerticalString(AlignVertical::Top), 
					mWidget->mAlignVertical == AlignVertical::Top))
				{
					mWidget->mAlignVertical = AlignVertical::Top;
				}

				if (layout.comboItem(getAlignVerticalString(AlignVertical::Bottom), 
					mWidget->mAlignVertical == AlignVertical::Bottom))
				{
					mWidget->mAlignVertical = AlignVertical::Bottom;
				}

				if (layout.comboItem(getAlignVerticalString(AlignVertical::Center), 
					mWidget->mAlignVertical == AlignVertical::Center))
				{
					mWidget->mAlignVertical = AlignVertical::Center;
				}

				mGui->layoutWidgets();
				layout.endCombo();
			}

			if (layout.beginCombo("Align Horizontal", getAlignHorizontalString(mWidget->mAlignHorizontal)))
			{
				if (layout.comboItem(getAlignHorizontalString(AlignHorizontal::DontCare),
					mWidget->mAlignHorizontal == AlignHorizontal::DontCare))
				{
					mWidget->mAlignHorizontal = AlignHorizontal::DontCare;
				}

				if (layout.comboItem(getAlignHorizontalString(AlignHorizontal::Left),
					mWidget->mAlignHorizontal == AlignHorizontal::Left))
				{
					mWidget->mAlignHorizontal = AlignHorizontal::Left;
				}

				if (layout.comboItem(getAlignHorizontalString(AlignHorizontal::Right),
					mWidget->mAlignHorizontal == AlignHorizontal::Right))
				{
					mWidget->mAlignHorizontal = AlignHorizontal::Right;
				}

				if (layout.comboItem(getAlignHorizontalString(AlignHorizontal::Center),
					mWidget->mAlignHorizontal == AlignHorizontal::Center))
				{
					mWidget->mAlignHorizontal = AlignHorizontal::Center;
				}

				mGui->layoutWidgets();
				layout.endCombo();
			}

			if (layout.beginCombo("Fill Mode", getFillModeString(mWidget->mFillMode)))
			{
				if (layout.comboItem(getFillModeString(FillMode::DontCare), 
					mWidget->mFillMode == FillMode::DontCare))
				{
					mWidget->mFillMode = FillMode::DontCare;
				}

				if (layout.comboItem(getFillModeString(FillMode::Horizontal),
					mWidget->mFillMode == FillMode::Horizontal))
				{
					mWidget->mFillMode = FillMode::Horizontal;
				}

				if (layout.comboItem(getFillModeString(FillMode::Vertical),
					mWidget->mFillMode == FillMode::Vertical))
				{
					mWidget->mFillMode = FillMode::Vertical;
				}

				if (layout.comboItem(getFillModeString(FillMode::Both),
					mWidget->mFillMode == FillMode::Both))
				{
					mWidget->mFillMode = FillMode::Both;
				}

				mGui->layoutWidgets();
				layout.endCombo();
			}

			layout.endLayout();
		}
	}

	void WidgetEditor::addTransform(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (auto* editor = mWidget->mTransform.getEditor(*mGui); editor != nullptr)
		{
			editor->onInspectorGui(layout, cache);
		}
	}

	const char* WidgetEditor::getAlignVerticalString(AlignVertical alignVertical) const
	{
		switch (alignVertical)
		{
		case AlignVertical::DontCare:	return "Don't Care";
		case AlignVertical::Top:		return "Top";
		case AlignVertical::Bottom:		return "Bottom";
		case AlignVertical::Center:		return "Center";
		}

		return "";
	}

	const char* WidgetEditor::getAlignHorizontalString(AlignHorizontal alignHorizontal) const
	{
		switch (alignHorizontal)
		{
		case AlignHorizontal::DontCare:		return "Don't Care";
		case AlignHorizontal::Left:			return "Left";
		case AlignHorizontal::Right:		return "Right";
		case AlignHorizontal::Center:		return "Center";
		}

		return "";
	}

	const char* WidgetEditor::getFillModeString(FillMode fillMode) const
	{
		switch (fillMode)
		{
		case FillMode::DontCare:		return "Don't Care";
		case FillMode::Horizontal:		return "Horizontal";
		case FillMode::Vertical:		return "Vertical";
		case FillMode::Both:			return "Both";
		}

		return "";
	}

	void WidgetSerializer::setWidget(Widget& widget)
	{
		mWidget = &widget;
	}

	void WidgetSerializer::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	bool WidgetSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		auto uuid = UUIDv4::UUID::fromStrFactory(reader.readString());
		auto name = reader.readString();

		mGui->updateWidgetUUID(mWidget, uuid);
		mWidget->mName = name;

		auto parentUUID = reader.readString();
		if (!parentUUID.empty())
		{
			auto* parent = mGui->findWidget(UUIDv4::UUID::fromStrFactory(parentUUID));
			if (!parent)
			{
				LogError("Unable to find parent in gui with uuid: '%s'", parentUUID.c_str());
				return false;
			}
			else
			{
				parent->addChild(*mWidget);
			}
		}

		if (!reader.read(glm::value_ptr(mWidget->mPosition)))
		{
			LogError("FileReader::read() failed for 'position'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mWidget->mSize)))
		{
			LogError("FileReader::read() failed for 'size'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mWidget->mPadding)))
		{
			LogError("FileReader::read() failed for 'padding'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mWidget->mMargin)))
		{
			LogError("FileReader::read() failed for 'margin'");
			return false;
		}

		if (!reader.read(&mWidget->mEnabled))
		{
			LogError("FileReader::read() failed for 'enabled'");
			return false;
		}

		if (!reader.read(&mWidget->mVisible))
		{
			LogError("FileReader::read() failed for 'visible'");
			return false;
		}

		if (!reader.read((uint32_t*)&mWidget->mAlignVertical))
		{
			LogError("FileReader::read() failed for 'align vertical'");
			return false;
		}

		if (!reader.read((uint32_t*)&mWidget->mAlignHorizontal))
		{
			LogError("FileReader::read() failed for 'align horizontal'");
			return false;
		}

		if (!reader.read((uint32_t*)&mWidget->mFillMode))
		{
			LogError("FileReader::read() failed for 'fill mode'");
			return false;
		}

		if (auto* serializer = mWidget->mTransform.getSerializer(*mGui); serializer != nullptr)
		{
			if (!serializer->read(reader, cache))
			{
				LogError("WidgetTransformSerializer::read() failed for 'transform'");
				return false;
			}
		}

		return true;
	}

	bool WidgetSerializer::write(FileWriter& writer)
	{
		if (!writer.writeString(mWidget->getUUID().str()))
		{
			LogError("FileWriter::writeString() failed for 'widget UUID'");
			return false;
		}

		if (!writer.writeString(mWidget->mName))
		{
			LogError("FileWriter::writeString() failed for 'widget name'");
			return false;
		}

		if (mWidget->mParent != nullptr)
		{
			if (!writer.writeString(mWidget->mParent->mUUID.str()))
			{
				LogError("FileWriter::write() failed for 'widget parent'");
				return false;
			}
		}

		if (!writer.write(glm::value_ptr(mWidget->mPosition)))
		{
			LogError("FileWriter::write() failed for 'position'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mWidget->mSize)))
		{
			LogError("FileWriter::write() failed for 'size'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mWidget->mPadding)))
		{
			LogError("FileWriter::write() failed for 'padding'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mWidget->mMargin)))
		{
			LogError("FileWriter::write() failed for 'margin'");
			return false;
		}

		if (!writer.write(&mWidget->mEnabled))
		{
			LogError("FileWriter::write() failed for 'enabled'");
			return false;
		}

		if (!writer.write(&mWidget->mVisible))
		{
			LogError("FileWriter::write() failed for 'visible'");
			return false;
		}

		if (!writer.write((const uint32_t*)&mWidget->mAlignVertical))
		{
			LogError("FileWriter::write() failed for 'align vertical'");
			return false;
		}

		if (!writer.write((const uint32_t*)&mWidget->mAlignHorizontal))
		{
			LogError("FileWriter::write() failed for 'align horizontal'");
			return false;
		}

		if (!writer.write((const uint32_t*)&mWidget->mFillMode))
		{
			LogError("FileWriter::write() failed for 'fill mode'");
			return false;
		}

		if (auto* serializer = mWidget->mTransform.getSerializer(*mGui); serializer != nullptr)
		{
			if (!serializer->write(writer))
			{
				LogError("WidgetTransformSerializer::write() failed for 'transform'");
				return false;
			}
		}

		return true;
	}

	bool WidgetSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("uuid"))
		{
			LogError("JSON Widget object doesn't contain 'uuid' key");
			return false;
		}

		if (!object.contains("name"))
		{
			LogError("JSON Widget object doesn't contain 'name' key");
			return false;
		}

		if (!object.contains("parent"))
		{
			LogError("JSON Widget object doesn't contain 'parent' key");
			return false;
		}

		auto uuid = UUIDv4::UUID::fromStrFactory(object["uuid"].get<std::string>());
		auto name = object["name"].get<std::string>();

		mGui->updateWidgetUUID(mWidget, uuid);
		mWidget->mName = name;

		auto parentUUID = object["parent"].get<std::string>();
		if (!parentUUID.empty())
		{
			auto* parent = mGui->findWidget(UUIDv4::UUID::fromStrFactory(parentUUID));
			if (!parent)
			{
				LogError("Unable to find parent in gui with uuid: '%s'", parentUUID.c_str());
				return false;
			}
			else
			{
				parent->addChild(*mWidget);
			}
		}

		mWidget->mPosition = {
			object["position"][0].get<float>(),
			object["position"][1].get<float>()
		};

		mWidget->mSize = {
			object["size"][0].get<float>(),
			object["size"][1].get<float>()
		};

		mWidget->mPadding = {
			object["padding"][0].get<float>(),
			object["padding"][1].get<float>(),
			object["padding"][2].get<float>(),
			object["padding"][3].get<float>()
		};

		mWidget->mMargin = {
			object["margin"][0].get<float>(),
			object["margin"][1].get<float>(),
			object["margin"][2].get<float>(),
			object["margin"][3].get<float>()
		};

		mWidget->mEnabled = object["enabled"].get<bool>();
		mWidget->mVisible = object["visible"].get<bool>();
		mWidget->mAlignVertical = (AlignVertical)object["alignVertical"].get<uint32_t>();
		mWidget->mAlignHorizontal = (AlignHorizontal)object["alignHorizontal"].get<uint32_t>();
		mWidget->mFillMode = (FillMode)object["fillMode"].get<uint32_t>();

		if (auto* serializer = mWidget->mTransform.getSerializer(*mGui); serializer != nullptr)
		{
			if (!serializer->read(object["transform"], cache))
			{
				LogError("WidgetTransformSerializer::read() failed for 'transform'");
				return false;
			}
		}

		return true;
	}

	bool WidgetSerializer::write(json& object)
	{
		std::string parentUUID;
		if (mWidget->mParent != nullptr)
		{
			parentUUID = mWidget->mParent->mUUID.str();
		}

		object["uuid"] = mWidget->mUUID.str();
		object["name"] = mWidget->mName;
		object["parent"] = parentUUID;

		object["position"] = std::vector<float>{
			mWidget->mPosition.x,
			mWidget->mPosition.y
		};

		object["size"] = std::vector<float>{
			mWidget->mSize.x,
			mWidget->mSize.y
		};

		object["padding"] = std::vector<float>{
			mWidget->mPadding.x,
			mWidget->mPadding.y,
			mWidget->mPadding.z,
			mWidget->mPadding.w
		};

		object["margin"] = std::vector<float>{
			mWidget->mMargin.x,
			mWidget->mMargin.y,
			mWidget->mMargin.z,
			mWidget->mMargin.w
		};

		object["enabled"] = mWidget->mEnabled;
		object["visible"] = mWidget->mVisible;
		object["alignVertical"] = (uint32_t)mWidget->mAlignVertical;
		object["alignHorizontal"] = (uint32_t)mWidget->mAlignHorizontal;
		object["fillMode"] = (uint32_t)mWidget->mFillMode;

		if (auto* serializer = mWidget->mTransform.getSerializer(*mGui); serializer != nullptr)
		{
			object["transform"] = {};
			if (!serializer->write(object["transform"]))
			{
				LogError("WidgetTransformSerializer::write() failed for 'transform'");
				return false;
			}
		}

		return true;
	}
}