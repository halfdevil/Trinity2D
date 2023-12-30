#include "Scene/Components/Collider.h"
#include "Scene/Components/RigidBody.h"
#include "Scene/Components/Transform.h"
#include "Scene/Node.h"
#include "Physics/RectangleShape.h"
#include "Physics/CircleShape.h"
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

		mQuadTreeData = {
			.collider = this
		};

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

	void Collider::setColliders(std::vector<Collider*>&& colliders)
	{
		for (auto* collider : mColliders)
		{
			if (std::find(colliders.begin(), colliders.end(), collider) == colliders.end())
			{
				onCollisionExit.notify(*collider);
			}
		}

		for (auto* collider: colliders)
		{
			if (std::find(mColliders.begin(), mColliders.end(), collider) == colliders.end())
			{
				onCollisionEnter.notify(*collider);
			}
		}

		for (auto* collider : mColliders)
		{
			if (std::find(colliders.begin(), colliders.end(), collider) != colliders.end())
			{
				onCollisionStay.notify(*collider);
			}
		}

		mColliders = std::move(colliders);
	}

	bool Collider::hasLayer(uint32_t layerIdx) const
	{
		return mLayers & (1 << layerIdx);
	}

	void Collider::addLayer(uint32_t layerIdx)
	{
		mLayers |= (1 << layerIdx);
	}

	void Collider::removeLayer(uint32_t layerIdx)
	{
		mLayers &= ~(1 << layerIdx);
	}

	void Collider::update()
	{
		if (mRigidBody != nullptr)
		{			
			mQuadTreeData.bounds = mRigidBody->getBounds();
		}		
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

		if (!reader.read(&mCollider->mLayers))
		{
			LogError("FileReader::read() failed for 'layers'");
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

		if (!writer.write(&mCollider->mLayers))
		{
			LogError("FileWriter::write() failed for 'layers'");
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

		if (!object.contains("layers"))
		{
			LogError("JSON collider object doesn't have 'layers' key");
			return false;
		}

		mCollider->mLayers = object["layers"].get<uint32_t>();
		return true;
	}

	bool ColliderSerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		object["layers"] = mCollider->mLayers;
		return true;
	}
}