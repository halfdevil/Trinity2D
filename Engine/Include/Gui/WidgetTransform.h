#pragma once

#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include "glm/glm.hpp"

namespace Trinity
{
	class Gui;
	class Widget;
	class WidgetTransformEditor;
	class WidgetTransformSerializer;

	class WidgetTransform
	{
	public:

		friend class WidgetTransformEditor;
		friend class WidgetTransformSerializer;

		WidgetTransform() = default;
		virtual ~WidgetTransform() = default;

		WidgetTransform(const WidgetTransform&) = delete;
		WidgetTransform& operator = (const WidgetTransform&) = delete;

		WidgetTransform(WidgetTransform&&) = default;
		WidgetTransform& operator = (WidgetTransform&&) = default;

		const glm::vec2& getTranslation() const
		{
			return mTranslation;
		}

		const glm::vec2& getScale() const
		{
			return mScale;
		}

		float getRotation() const
		{
			return mRotation;
		}

		Widget* getWidget() const
		{
			return mWidget;
		}

		virtual glm::mat4 getMatrix() const;
		virtual glm::mat4 getWorldMatrix();

		virtual IEditor* getEditor(Gui& gui);
		virtual ISerializer* getSerializer(Gui& gui);

		virtual void setWidget(Widget& widget);
		virtual void setMatrix(const glm::mat4& matrix);
		virtual void setWorldMatrix(const glm::mat4& matrix);

		virtual void setTranslation(const glm::vec2& translation);
		virtual void setScale(const glm::vec2& scale);
		virtual void setRotation(float rotation);
		virtual void invalidateWorldMatrix();

	protected:

		void updateWorldTransform();

	protected:

		glm::vec2 mTranslation{ 0.0f };
		glm::vec2 mScale{ 1.0f };
		float mRotation{ 0.0f };
		glm::mat4 mWorldMatrix{ 1.0f };
		Widget* mWidget{ nullptr };

	private:

		bool mUpdateMatrix{ true };
	};

	class WidgetTransformEditor : public IEditor
	{
	public:

		WidgetTransformEditor() = default;
		virtual ~WidgetTransformEditor() = default;

		virtual void setWidgetTransform(WidgetTransform& transform);
		virtual void setGui(Gui& gui);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		WidgetTransform* mTransform{ nullptr };
		Gui* mGui{ nullptr };
	};

	class WidgetTransformSerializer : public ISerializer
	{
	public:

		WidgetTransformSerializer() = default;
		virtual ~WidgetTransformSerializer() = default;

		WidgetTransformSerializer(const WidgetTransformSerializer&) = delete;
		WidgetTransformSerializer& operator = (const WidgetTransformSerializer&) = delete;

		WidgetTransformSerializer(WidgetTransformSerializer&&) = default;
		WidgetTransformSerializer& operator = (WidgetTransformSerializer&&) = default;

		virtual void setWidgetTransform(WidgetTransform& transform);
		virtual void setGui(Gui& gui);

		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		WidgetTransform* mTransform{ nullptr };
		Gui* mGui{ nullptr };
	};
}