#pragma once

#include "Scene/Component.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class CameraEditor;
	class CameraSerializer;

	class Camera : public Component
	{
	public:

		friend class CameraEditor;
		friend class CameraSerializer;

		Camera() = default;
		virtual ~Camera() = default;

		Camera(const Camera&) = delete;
		Camera& operator = (const Camera&) = delete;

		Camera(Camera&&) = default;
		Camera& operator = (Camera&&) = default;

		const glm::vec2& getSize() const
		{
			return mSize;
		}

		virtual glm::mat4 getView() const;
		virtual glm::mat4 getProjection() const;

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getUUID() const override;

		virtual Editor* getEditor() override;
		virtual Serializer* getSerializer(Scene& scene) override;

		virtual void setSize(const glm::vec2& size);
		virtual void setNearPlane(float nearPlane);
		virtual void setFarPlane(float farPlane);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("ddc4737c-17aa-4928-9d60-6041eb96b562");

	protected:

		glm::vec2 mSize{ 0.0f };
		float mNearPlane{ 0.1f };
		float mFarPlane{ 100.0f };
	};

	class CameraEditor : public Editor
	{
	public:

		CameraEditor() = default;
		virtual ~CameraEditor() = default;

		CameraEditor(const CameraEditor&) = delete;
		CameraEditor& operator = (const CameraEditor&) = delete;

		CameraEditor(CameraEditor&&) = default;
		CameraEditor& operator = (CameraEditor&&) = default;

		virtual void setCamera(Camera& camera);
		virtual void onInspectorGui(const EditorLayout& layout) override;

	protected:

		Camera* mCamera{ nullptr };
	};

	class CameraSerializer : public ComponentSerializer
	{
	public:

		CameraSerializer() = default;
		virtual ~CameraSerializer() = default;

		CameraSerializer(const CameraSerializer&) = delete;
		CameraSerializer& operator = (const CameraSerializer&) = delete;

		CameraSerializer(CameraSerializer&&) = default;
		CameraSerializer& operator = (CameraSerializer&&) = default;

		virtual void setCamera(Camera& camera);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Camera* mCamera{ nullptr };
	};
}