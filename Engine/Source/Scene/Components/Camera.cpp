#include "Scene/Components/Camera.h"
#include "Scene/Node.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"
#include "glm/gtc/type_ptr.hpp"

namespace Trinity
{
	glm::mat4 Camera::getView() const
	{
		auto& transform = mNode->getTransform();
		return glm::inverse(transform.getWorldMatrix());
	}

	glm::mat4 Camera::getProjection() const
	{
		float halfWidth = 0.5f * mSize.x;
		float halfHeight = 0.5f * mSize.y;

		return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight);
	}

	std::type_index Camera::getType() const
	{
		return typeid(Camera);
	}

	UUIDv4::UUID Camera::getTypeUUID() const
	{
		return Camera::UUID;
	}

	IEditor* Camera::getEditor(Scene& scene)
	{
		static CameraEditor editor;
		editor.setCamera(*this);
		editor.setScene(scene);

		return &editor;
	}

	ISerializer* Camera::getSerializer(Scene& scene)
	{
		static CameraSerializer serializer;
		serializer.setScene(scene);
		serializer.setCamera(*this);

		return &serializer;
	}

	void Camera::setSize(const glm::vec2& size)
	{
		mSize = size;
	}

	void CameraEditor::setCamera(Camera& camera)
	{
		mCamera = &camera;
		setComponent(camera);
	}

	void CameraEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Camera"))
		{
			addCommonFields(layout);

			layout.inputVec2("Size", mCamera->mSize);
			layout.endLayout();
		}
	}

	void CameraSerializer::setCamera(Camera& camera)
	{
		mCamera = &camera;
		setComponent(camera);
	}

	bool CameraSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!reader.read(glm::value_ptr(mCamera->mSize)))
		{
			LogError("FileReader::read() failed for 'size'");
			return false;
		}

		return true;
	}

	bool CameraSerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		if (!writer.write(glm::value_ptr(mCamera->mSize)))
		{
			LogError("FileWriter::write() failed for 'size'");
			return false;
		}

		return true;
	}

	bool CameraSerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!object.contains("size"))
		{
			LogError("JSON Camera object doesn't contain 'size' key");
			return false;
		}

		mCamera->mSize = { 
			object["size"][0].get<float>(), 
			object["size"][1].get<float>()
		};

		return true;
	}

	bool CameraSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		object["size"] = std::vector<float>{
			mCamera->mSize.x, 
			mCamera->mSize.y
		};

		return true;
	}
}