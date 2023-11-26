#pragma once

#include "glm/glm.hpp"

namespace Trinity
{
	class Camera;

	enum GizmoMode : uint32_t
	{
		Local = 0,
		Global
	};

	enum GizmoOperation : uint32_t
	{
		TranslateX = (1u << 0),
		TranslateY = (1u << 1),
		Rotate = (1u << 5),
		ScaleX = (1u << 7),
		ScaleY = (1u << 8),
		Translate = TranslateX | TranslateY,
		Scale = ScaleX | ScaleY
	};

	class EditorGizmo
	{
	public:

		EditorGizmo() = default;
		virtual ~EditorGizmo() = default;

		EditorGizmo(const EditorGizmo&) = delete;
		EditorGizmo& operator = (const EditorGizmo&) = delete;

		EditorGizmo(EditorGizmo&&) = default;
		EditorGizmo& operator = (EditorGizmo&&) = default;

		GizmoMode getMode() const
		{
			return mMode;
		}

		GizmoOperation getOperation() const
		{
			return mOperation;
		}

		virtual void setMode(GizmoMode mode);
		virtual void setOperation(GizmoOperation operation);
		virtual void setCamera(Camera& camera);

		virtual void setRect(float x, float y, float width, float height);
		virtual bool show(glm::mat4& matrix);

	protected:

		GizmoMode mMode{ GizmoMode::Local };
		GizmoOperation mOperation{ GizmoOperation::Translate };
		Camera* mCamera{ nullptr };
	};
}