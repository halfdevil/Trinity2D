#pragma once

#include "Scene/Component.h"
#include "Editor/Editor.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class CameraEditor;

	class Camera : public Component
	{
	public:

		friend class CameraEditor;

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
		virtual std::string getTypeName() const override;
		virtual Editor* getEditor() override;

		virtual void setSize(const glm::vec2& size);
		virtual void setNearPlane(float nearPlane);
		virtual void setFarPlane(float farPlane);

	public:

		static std::string getStaticType();

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
}