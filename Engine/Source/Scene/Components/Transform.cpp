#include "Scene/Components/Transform.h"
#include "Scene/Node.h"
#include "Scene/Scene.h"
#include "Editor/EditorLayout.h"
#include "Math/Math.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	std::type_index Transform::getType() const
	{
		return typeid(Transform);
	}

	UUIDv4::UUID Transform::getTypeUUID() const
	{
		return Transform::UUID;
	}

	IEditor* Transform::getEditor(Scene& scene)
	{
		static TransformEditor editor;
		editor.setScene(scene);
		editor.setTransform(*this);

		return &editor;
	}

	ISerializer* Transform::getSerializer(Scene& scene)
	{
		static TransformSerializer serializer;
		serializer.setTransform(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	glm::mat4 Transform::getMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(mTranslation, 0.0f)) *
			glm::yawPitchRoll(0.0f, 0.0f, mRotation) *
			glm::scale(glm::mat4(1.0f), glm::vec3(mScale, 1.0f));
	}

	glm::mat4 Transform::getWorldMatrix()
	{
		updateWorldTransform();
		return mWorldMatrix;
	}

	void Transform::setMatrix(const glm::mat4& matrix)
	{
		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };

		Math::decomposeTransform(matrix, translation, rotation, scale);

		mTranslation = glm::vec2(translation);
		mRotation = rotation.z;
		mScale = glm::vec2(scale);

		invalidateWorldMatrix();
	}

	void Transform::setWorldMatrix(const glm::mat4& matrix)
	{
		auto parent = mNode->getParent();
		if (parent != nullptr)
		{
			auto& transform = parent->getTransform();
			setMatrix(glm::inverse(transform.getWorldMatrix()) * matrix);
		}
		else
		{
			setMatrix(matrix);
		}

		mWorldMatrix = matrix;
	}

	void Transform::setWorldMatrix(const glm::vec2& translation, float rotation, const glm::vec2& scale)
	{
		mWorldMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation, 0.0f)) *
			glm::yawPitchRoll(0.0f, 0.0f, rotation) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale, 1.0f));

		auto parent = mNode->getParent();
		if (parent != nullptr)
		{
			auto& transform = parent->getTransform();
			setMatrix(glm::inverse(transform.getWorldMatrix()) * mWorldMatrix);
		}
		else
		{
			setMatrix(mWorldMatrix);
		}
	}

	void Transform::setTranslation(const glm::vec2& translation)
	{
		mTranslation = translation;
		invalidateWorldMatrix();
	}

	void Transform::setRotation(float rotation)
	{
		mRotation = rotation;
		invalidateWorldMatrix();
	}

	void Transform::setScale(const glm::vec2& scale)
	{
		mScale = scale;
		invalidateWorldMatrix();
	}

	void Transform::invalidateWorldMatrix()
	{
		mUpdateMatrix = true;

		auto& children = mNode->getChildren();
		for (auto* child : children)
		{
			auto& transform = child->getTransform();
			transform.invalidateWorldMatrix();
		}
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

	void TransformEditor::setTransform(Transform& transform)
	{
		mTransform = &transform;
		setComponent(transform);
	}

	void TransformEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Transform"))
		{
			if (layout.inputVec2("Translation", mTransform->mTranslation))
			{
				mTransform->invalidateWorldMatrix();
			}

			if (layout.inputFloat("Rotation", mTransform->mRotation))
			{
				mTransform->invalidateWorldMatrix();
			}

			if (layout.inputVec2("Scale", mTransform->mScale))
			{
				mTransform->invalidateWorldMatrix();
			}

			layout.endLayout();
		}
	}

	void TransformSerializer::setTransform(Transform& transform)
	{
		mTransform = &transform;
		setComponent(transform);
	}

	bool TransformSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!reader.read(glm::value_ptr(mTransform->mTranslation)))
		{
			LogError("FileReader::read() failed for 'translation'");
			return false;
		}

		if (!reader.read(&mTransform->mRotation))
		{
			LogError("FileReader::read() failed for 'rotation'");
			return false;
		}

		if (!reader.read(glm::value_ptr(mTransform->mScale)))
		{
			LogError("FileReader::read() failed for 'scale'");
			return false;
		}

		return true;
	}

	bool TransformSerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		if (!writer.write(glm::value_ptr(mTransform->mTranslation)))
		{
			LogError("FileWriter::write() failed for 'translation'");
			return false;
		}

		if (!writer.write(&mTransform->mRotation))
		{
			LogError("FileWriter::write() failed for 'rotation'");
			return false;
		}

		if (!writer.write(glm::value_ptr(mTransform->mScale)))
		{
			LogError("FileWriter::write() failed for 'scale'");
			return false;
		}

		return true;
	}

	bool TransformSerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!object.contains("translation"))
		{
			LogError("JSON Transform object doesn't contain 'translation' key");
			return false;
		}

		if (!object.contains("rotation"))
		{
			LogError("JSON Transform object doesn't contain 'rotation' key");
			return false;
		}

		if (!object.contains("scale"))
		{
			LogError("JSON Transform object doesn't contain 'scale' key");
			return false;
		}

		mTransform->mTranslation = {
			object["translation"][0].get<float>(),
			object["translation"][1].get<float>()
		};

		mTransform->mRotation =	object["rotation"].get<float>();
		mTransform->mScale = {
			object["scale"][0].get<float>(),
			object["scale"][1].get<float>()
		};

		return true;
	}

	bool TransformSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		object["translation"] = std::vector<float>{
			mTransform->mTranslation.x,
			mTransform->mTranslation.y
		};

		object["rotation"] = mTransform->mRotation;
		object["scale"] = std::vector<float>{
			mTransform->mScale.x,
			mTransform->mScale.y
		};

		return true;
	}
}