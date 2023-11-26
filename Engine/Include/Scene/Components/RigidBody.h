#pragma once

#include "Scene/Component.h"
#include "Physics/RigidShape.h"
#include "Editor/Editor.h"
#include "VFS/Serializer.h"

namespace Trinity
{
	class SpriteRenderable;
	class TextureRenderable;
	class RigidBodyEditor;
	class RigidBodySerializer;

	class RigidBody : public Component
	{
	public:

		friend class RigidBodyEditor;
		friend class RigidBodySerializer;

		RigidBody() = default;
		virtual ~RigidBody() = default;

		RigidBody(const RigidBody&) = delete;
		RigidBody& operator = (const RigidBody&) = delete;

		RigidBody(RigidBody&&) = default;
		RigidBody& operator = (RigidBody&&) = default;

		RigidShape* getShape() const
		{
			return mShape.get();
		}

		RigidShapeType getShapeType() const
		{
			return mShapeType;
		}

		float getMass() const
		{
			return mMass;
		}

		bool isKinematic() const
		{
			return mKinematic;
		}

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getUUID() const override;

		virtual IEditor* getEditor(Scene& scene) override;
		virtual ISerializer* getSerializer(Scene& scene) override;

		virtual bool init();
		virtual void update(float deltaTime);
		virtual void updateTransform();

		virtual void setShapeType(RigidShapeType shapeType);
		virtual void setMass(float mass);
		virtual void setKinematic(bool kinematic);

	protected:

		virtual void init(SpriteRenderable& renderable);
		virtual void init(TextureRenderable& renderable);

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("b3539446-856d-4f93-b2b8-c8527d9e3c34");

	protected:

		std::unique_ptr<RigidShape> mShape{ nullptr };
		RigidShapeType mShapeType{ RigidShapeType::Rectangle };
		float mMass{ 1.0f };
		bool mKinematic{ false };
	};

	class RigidBodyEditor : public ComponentEditor
	{
	public:

		RigidBodyEditor() = default;
		virtual ~RigidBodyEditor() = default;

		virtual void setRigidBody(RigidBody& rigidBody);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		RigidBody* mRigidBody{ nullptr };
	};

	class RigidBodySerializer : public ComponentSerializer
	{
	public:

		RigidBodySerializer() = default;
		virtual ~RigidBodySerializer() = default;

		RigidBodySerializer(const RigidBodySerializer&) = delete;
		RigidBodySerializer& operator = (const RigidBodySerializer&) = delete;

		RigidBodySerializer(RigidBodySerializer&&) = default;
		RigidBodySerializer& operator = (RigidBodySerializer&&) = default;

		virtual void setRigidBody(RigidBody& rigidBody);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		RigidBody* mRigidBody{ nullptr };
	};
}