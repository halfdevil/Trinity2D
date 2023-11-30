#include "Gui/WidgetTransform.h"
#include "Gui/Widget.h"
#include "Gui/Gui.h"
#include "Math/Math.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	glm::mat4 WidgetTransform::getMatrix() const
	{
		return glm::translate(glm::mat4(1.0f), glm::vec3(mTranslation, 0.0f)) *
			glm::yawPitchRoll(0.0f, 0.0f, mRotation) *
			glm::scale(glm::mat4(1.0f), glm::vec3(mScale, 1.0f));
	}

	glm::mat4 WidgetTransform::getWorldMatrix()
	{
		updateWorldTransform();
		return mWorldMatrix;
	}

	IEditor* WidgetTransform::getEditor(Gui& gui)
	{
		static WidgetTransformEditor editor;
		editor.setWidgetTransform(*this);
		editor.setGui(gui);

		return &editor;
	}

	ISerializer* WidgetTransform::getSerializer(Gui& gui)
	{
		static WidgetTransformSerializer serializer;
		serializer.setWidgetTransform(*this);
		serializer.setGui(gui);

		return &serializer;
	}

	void WidgetTransform::setWidget(Widget& widget)
	{
		mWidget = &widget;
	}

	void WidgetTransform::setMatrix(const glm::mat4& matrix)
	{
		glm::vec3 translation{ 0.0f };
		glm::vec3 rotation{ 0.0f };
		glm::vec3 scale{ 1.0f };

		Math::decomposeTransform(matrix, translation, rotation, scale);

		mTranslation = glm::vec2(translation);
		mScale = glm::vec2(scale);
		mRotation = rotation.z;
	}

	void WidgetTransform::setWorldMatrix(const glm::mat4& matrix)
	{
		auto parent = mWidget->getParent();
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

	void WidgetTransform::setTranslation(const glm::vec2& translation)
	{
		mTranslation = translation;
	}

	void WidgetTransform::setScale(const glm::vec2& scale)
	{
		mScale = scale;
	}

	void WidgetTransform::setRotation(float rotation)
	{
		mRotation = rotation;
	}

	void WidgetTransform::invalidateWorldMatrix()
	{
		mUpdateMatrix = true;
	}

	void WidgetTransform::updateWorldTransform()
	{
		if (!mUpdateMatrix)
		{
			return;
		}

		mWorldMatrix = getMatrix();

		auto parent = mWidget->getParent();
		if (parent != nullptr)
		{
			auto& transform = parent->getTransform();
			mWorldMatrix = transform.getWorldMatrix() * mWorldMatrix;
		}

		mUpdateMatrix = false;
	}

	void WidgetTransformEditor::setWidgetTransform(WidgetTransform& transform)
	{
		mTransform = &transform;
	}

	void WidgetTransformEditor::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	void WidgetTransformEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
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

	void WidgetTransformSerializer::setWidgetTransform(WidgetTransform& transform)
	{
		mTransform = &transform;
	}

	void WidgetTransformSerializer::setGui(Gui& gui)
	{
		mGui = &gui;
	}

	bool WidgetTransformSerializer::read(FileReader& reader, ResourceCache& cache)
	{
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

	bool WidgetTransformSerializer::write(FileWriter& writer)
	{
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

	bool WidgetTransformSerializer::read(json& object, ResourceCache& cache)
	{
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

		mTransform->mRotation = object["rotation"].get<float>();

		mTransform->mScale = {
			object["scale"][0].get<float>(),
			object["scale"][1].get<float>()
		};

		return true;
	}

	bool WidgetTransformSerializer::write(json& object)
	{
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