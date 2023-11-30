#include "Scene/Components/Collider.h"
#include "Scene/Components/RigidBody.h"
#include "Scene/Node.h"
#include "Editor/EditorLayout.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
	IEditor* Collider::getEditor(Scene& scene)
	{
		static ColliderEditor editor;
		editor.setScene(scene);
		editor.setCollider(*this);

		return &editor;
	}

	ISerializer* Collider::getSerializer(Scene& scene)
	{
		static ColliderSerializer serializer;
		serializer.setCollider(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	bool Collider::init()
	{
		if (mNode->hasComponent<RigidBody>())
		{
			mRigidBody = &mNode->getComponent<RigidBody>();
		}
		else
		{
			LogError("RigidBody missing from node");
			return false;
		}

		return true;
	}

	std::type_index Collider::getType() const
	{
		return typeid(Collider);
	}

	UUIDv4::UUID Collider::getTypeUUID() const
	{
		return Collider::UUID;
	}

	void ColliderEditor::setCollider(Collider& collider)
	{
		mCollider = &collider;
		setComponent(collider);
	}

	void ColliderEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{		
		if (layout.beginLayout("Collider"))
		{
			addCommonFields(layout);
			layout.endLayout();
		}
	}

	void ColliderSerializer::setCollider(Collider& collider)
	{
		mCollider = &collider;
	}

	bool ColliderSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		return true;
	}

	bool ColliderSerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		return true;
	}

	bool ColliderSerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		return true;
	}

	bool ColliderSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		return true;
	}
}