#include "Scene/Components/Transform.h"
#include "Scene/Node.h"
#include "Editor/EditorLayout.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/euler_angles.hpp"

namespace Trinity
{
	std::type_index Transform::getType() const
	{
		return typeid(Transform);
	}

	std::string Transform::getTypeName() const
	{
		return getStaticType();
	}

	Editor* Transform::getEditor()
	{
		static TransformEditor editor;
		editor.setTransform(*this);
		return &editor;
	}

	glm::mat4 Transform::getMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), mTranslation) *
			glm::yawPitchRoll(mRotation.y, mRotation.x, mRotation.z) *
			glm::scale(glm::mat4(1.0f), mScale);
	}

	glm::mat4 Transform::getWorldMatrix()
	{
		updateWorldTransform();
		return mWorldMatrix;
	}

	void Transform::setMatrix(const glm::mat4& matrix)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::quat rotation;

		glm::decompose(matrix, mScale, rotation, mTranslation, skew, perspective);
		mRotation = glm::eulerAngles(rotation);

		invalidateWorldMatrix();
	}

	void Transform::setTranslation(const glm::vec3& translation)
	{
		mTranslation = translation;
		invalidateWorldMatrix();
	}

	void Transform::setRotation(const glm::vec3& rotation)
	{
		mRotation = rotation;
		invalidateWorldMatrix();
	}

	void Transform::setScale(const glm::vec3& scale)
	{
		mScale = scale;
		invalidateWorldMatrix();
	}

	void Transform::invalidateWorldMatrix()
	{
		mUpdateMatrix = true;
	}

	void Transform::updateWorldTransform()
	{
		if (!mUpdateMatrix)
		{
			return;
		}

		mWorldMatrix = getMatrix();

		auto parent = mNode->getParent();
		if (parent != nullptr)
		{
			auto& transform = parent->getTransform();
			mWorldMatrix = transform.getWorldMatrix() * mWorldMatrix;
		}

		mUpdateMatrix = false;
	}

	std::string Transform::getStaticType()
	{
		return "Transform";
	}

	void TransformEditor::setTransform(Transform& transform)
	{
		mTransform = &transform;
	}

	void TransformEditor::onInspectorGui(const EditorLayout& layout)
	{
		if (layout.beginLayout("Transform"))
		{
			if (layout.inputVec3("Translation", mTransform->mTranslation))
			{
				mTransform->invalidateWorldMatrix();
			}

			if (layout.inputVec3("Rotation", mTransform->mRotation))
			{
				mTransform->invalidateWorldMatrix();
			}

			if (layout.inputVec3("Scale", mTransform->mScale))
			{
				mTransform->invalidateWorldMatrix();
			}

			layout.endLayout();
		}
	}
}