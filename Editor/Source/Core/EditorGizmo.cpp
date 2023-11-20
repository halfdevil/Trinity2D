#include "Core/EditorGizmo.h"
#include "Scene/Components/Camera.h"
#include "imgui.h"
#include "ImGuizmo.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	void EditorGizmo::setMode(GizmoMode mode)
	{
		mMode = mode;
	}

	void EditorGizmo::setOperation(GizmoOperation operation)
	{
		mOperation = operation;
	}

	void EditorGizmo::setCamera(Camera& camera)
	{
		mCamera = &camera;
	}

	void EditorGizmo::setUseOrigin(bool useOrigin)
	{
		mUseOrigin = useOrigin;
	}

	void EditorGizmo::setRect(float x, float y, float width, float height)
	{
		ImGuizmo::SetRect(x, y, width, height);
	}

	bool EditorGizmo::show(glm::mat4& matrix, const glm::vec2& origin, const glm::vec2& size)
	{
		ImGuizmo::SetDrawlist();

		if (mUseOrigin)
		{
			glm::vec3 t{ size.x * origin.x, size.y * origin.y, 0.0f };
			matrix = glm::translate(matrix, t);
		}

		auto operation = (ImGuizmo::OPERATION)mOperation;
		auto mode = (ImGuizmo::MODE)mMode;

		ImGuizmo::Manipulate(
			glm::value_ptr(mCamera->getView()), 
			glm::value_ptr(mCamera->getProjection()), 
			operation, 
			mode, 
			glm::value_ptr(matrix)
		);

		if (ImGuizmo::IsUsing())
		{
			if (mUseOrigin)
			{
				glm::vec3 t{ size.x * -origin.x, size.y * -origin.y, 0.0f };
				matrix = glm::translate(matrix, t);
			}

			return true;
		}

		return false;
	}
}