#pragma once

#include "Scene/Component.h"
#include "Scene/QuadTree.h"
#include "Core/Observer.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"

namespace Trinity
{
	class RigidBody;
	class Collider;
	class ColliderEditor;
	class ColliderSerializer;
	struct CollisionInfo;

	struct ColliderData : public QuadTreeData
	{
		Collider* collider{ nullptr };
	};

	class Collider : public Component
	{
	public:

		friend class ColliderEditor;
		friend class ColliderSerializer;

		Collider() = default;
		virtual ~Collider() = default;

		Collider(const Collider&) = delete;
		Collider& operator = (const Collider&) = delete;

		Collider(Collider&&) = default;
		Collider& operator = (Collider&&) = default;

		RigidBody* getRigidBody() const
		{
			return mRigidBody;
		}

		uint32_t getLayers() const
		{
			return mLayers;
		}

		ColliderData& getQuadTreeData()
		{
			return mQuadTreeData;
		}

		virtual IEditor* getEditor(Scene& scene) override;
		virtual ISerializer* getSerializer(Scene& scene) override;

		virtual bool init();
		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual void setColliders(std::vector<Collider*>&& colliders);
		virtual bool hasLayer(uint32_t layerIdx) const;
		virtual void addLayer(uint32_t layerIdx);
		virtual void removeLayer(uint32_t layerIdx);
		virtual void update();

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("ba71841c-9881-4792-85a1-c3c886440c33");

	public:

		Observer<const Collider&> onCollisionEnter;
		Observer<const Collider&> onCollisionExit;
		Observer<const Collider&> onCollisionStay;

	protected:

		RigidBody* mRigidBody{ nullptr };
		ColliderData mQuadTreeData;
		uint32_t mLayers{ 0 };
		std::vector<Collider*> mColliders;
	};

	class ColliderEditor : public ComponentEditor
	{
	public:

		ColliderEditor() = default;
		virtual ~ColliderEditor() = default;

		virtual void setCollider(Collider& collider);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		Collider* mCollider{ nullptr };
	};

	class ColliderSerializer : public ComponentSerializer
	{
	public:

		ColliderSerializer() = default;
		virtual ~ColliderSerializer() = default;

		ColliderSerializer(const ColliderSerializer&) = delete;
		ColliderSerializer& operator = (const ColliderSerializer&) = delete;

		ColliderSerializer(ColliderSerializer&&) = default;
		ColliderSerializer& operator = (ColliderSerializer&&) = default;

		virtual void setCollider(Collider& collider);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Collider* mCollider{ nullptr };
	};
}