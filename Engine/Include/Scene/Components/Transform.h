#pragma once

#include "Scene/Component.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Trinity
{
	class Node;
	class TransformEditor;
	class TransformSerializer;

	class Transform : public Component
	{
	public:

		friend class TransformEditor;
		friend class TransformSerializer;

		Transform() = default;
		virtual ~Transform() = default;

		Transform(const Transform&) = delete;
		Transform& operator = (const Transform&) = delete;

		Transform(Transform&&) = default;
		Transform& operator = (Transform&&) = default;

		const glm::vec2& getTranslation() const
		{
			return mTranslation;
		}

		float getRotation() const
		{
			return mRotation;
		}

		const glm::vec2& getScale() const
		{
			return mScale;
		}

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual IEditor* getEditor(Scene& scene) override;
		virtual ISerializer* getSerializer(Scene& scene) override;

		virtual glm::mat4 getMatrix() const;
		virtual glm::mat4 getWorldMatrix();

		virtual void setMatrix(const glm::mat4& matrix);
		virtual void setWorldMatrix(const glm::mat4& matrix);
		virtual void setWorldMatrix(const glm::vec2& translation, float rotation,
			const glm::vec2& scale);

		virtual void setTranslation(const glm::vec2& translation);
		virtual void setRotation(float rotation);
		virtual void setScale(const glm::vec2& scale);
		virtual void invalidateWorldMatrix();

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("173f2ec0-93c2-4be1-bf11-f25e61ce2bdb");

	protected:

		void updateWorldTransform();

	protected:

		glm::vec2 mTranslation{ 0.0f };
		float mRotation{ 0.0f };
		glm::vec2 mScale{ 1.0f };
		glm::mat4 mWorldMatrix{ 1.0f };

	private:

		bool mUpdateMatrix{ true };
	};

	class TransformEditor : public ComponentEditor
	{
	public:

		TransformEditor() = default;
		virtual ~TransformEditor() = default;

		virtual void setTransform(Transform& transform);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		Transform* mTransform{ nullptr };
	};

	class TransformSerializer : public ComponentSerializer
	{
	public:

		TransformSerializer() = default;
		virtual ~TransformSerializer() = default;

		TransformSerializer(const TransformSerializer&) = delete;
		TransformSerializer& operator = (const TransformSerializer&) = delete;

		TransformSerializer(TransformSerializer&&) = default;
		TransformSerializer& operator = (TransformSerializer&&) = default;

		virtual void setTransform(Transform& transform);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Transform* mTransform{ nullptr };
	};
}