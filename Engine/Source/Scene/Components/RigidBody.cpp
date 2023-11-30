#include "Scene/Components/RigidBody.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Components/TextureRenderable.h"
#include "Scene/Components/Transform.h"
#include "Scene/Node.h"
#include "Scene/Sprite.h"
#include "Editor/EditorLayout.h"
#include "Physics/RectangleShape.h"
#include "Physics/CircleShape.h"
#include "Graphics/Texture.h"
#include "VFS/FileReader.h"
#include "VFS/FileWriter.h"
#include "Core/Logger.h"

namespace Trinity
{
	std::type_index RigidBody::getType() const
	{
		return typeid(RigidBody);
	}

	UUIDv4::UUID RigidBody::getTypeUUID() const
	{
		return RigidBody::UUID;
	}

	IEditor* RigidBody::getEditor(Scene& scene)
	{
		static RigidBodyEditor editor;
		editor.setScene(scene);
		editor.setRigidBody(*this);

		return &editor;
	}

	ISerializer* RigidBody::getSerializer(Scene& scene)
	{
		static RigidBodySerializer serializer;
		serializer.setRigidBody(*this);
		serializer.setScene(scene);

		return &serializer;
	}

	bool RigidBody::init()
	{
		if (mNode->hasComponent<SpriteRenderable>())
		{
			init(mNode->getComponent<SpriteRenderable>());
		}
		else if (mNode->hasComponent<TextureRenderable>())
		{
			init(mNode->getComponent<TextureRenderable>());
		}
		else
		{
			LogError("SpriteRenderable/TextureRenderable missing from node");
			return false;
		}

		mShape->setMass(mMass);
		return true;
	}

	void RigidBody::update(float deltaTime)
	{
		if (mShape != nullptr)
		{ 
			mShape->update(deltaTime);
		}
	}

	void RigidBody::updateTransform()
	{
		if (mShape != nullptr)
		{
			auto& transform = mNode->getTransform();
			auto& translation = transform.getTranslation();
			auto& position = mShape->getPosition();

			transform.setTranslation(glm::vec3(position.x, position.y, translation.z));
			transform.setRotation(glm::vec3(0.0f, 0.0f, mShape->getAngle()));
		}
	}

	void RigidBody::setShapeType(RigidShapeType shapeType)
	{
		mShapeType = shapeType;
	}

	void RigidBody::setMass(float mass)
	{
		mMass = mass;
		mShape->setMass(mass);
	}

	void RigidBody::setKinematic(bool kinematic)
	{
		mKinematic = kinematic;
	}

	void RigidBody::init(SpriteRenderable& renderable)
	{
		auto& transform = mNode->getTransform();
		auto* sprite = renderable.getSprite();

		if (mShapeType == RigidShapeType::Rectangle)
		{
			auto shape = std::make_unique<RectangleShape>();
			shape->setPosition(transform.getTranslation());
			shape->setSize(sprite->getSize());
			shape->setOrigin(renderable.getOrigin());
			mShape = std::move(shape);
		}
		else
		{
			auto shape = std::make_unique<CircleShape>();
			shape->setPosition(transform.getTranslation());
			shape->setRadius(glm::length(sprite->getSize()) / 2.0f);
			mShape = std::move(shape);
		}
	}

	void RigidBody::init(TextureRenderable& renderable)
	{
		auto& transform = mNode->getTransform();
		auto* texture = renderable.getTexture();
		auto size = glm::vec2{ (float)texture->getWidth(), (float)texture->getHeight() };

		if (mShapeType == RigidShapeType::Rectangle)
		{
			auto shape = std::make_unique<RectangleShape>();			
			shape->setPosition(transform.getTranslation());
			shape->setSize(size);
			shape->setOrigin(renderable.getOrigin());
			mShape = std::move(shape);
		}
		else
		{
			auto shape = std::make_unique<CircleShape>();
			shape->setPosition(transform.getTranslation());
			shape->setRadius(glm::length(size) / 2.0f);
			mShape = std::move(shape);
		}
	}

	void RigidBodyEditor::setRigidBody(RigidBody& rigidBody)
	{
		mRigidBody = &rigidBody;
		setComponent(rigidBody);
	}

	void RigidBodyEditor::onInspectorGui(const IEditorLayout& layout, ResourceCache& cache)
	{
		if (layout.beginLayout("Rigid Body"))
		{
			addCommonFields(layout);

			auto& shapeType = mRigidBody->mShapeType;
			if (layout.beginCombo("Shape Type", shapeType == RigidShapeType::Circle ? 
				"Circle" : "Rectangle"))
			{
				if (layout.comboItem("Rectangle", shapeType == RigidShapeType::Rectangle))
				{
					shapeType = RigidShapeType::Rectangle;
				}

				if (layout.comboItem("Circle", shapeType == RigidShapeType::Circle))
				{
					shapeType = RigidShapeType::Circle;
				}

				layout.endCombo();
			}

			layout.inputFloat("Mass", mRigidBody->mMass);
			layout.endLayout();
		}
	}

	void RigidBodySerializer::setRigidBody(RigidBody& rigidBody)
	{
		mRigidBody = &rigidBody;
		setComponent(rigidBody);
	}

	bool RigidBodySerializer::read(FileReader& reader, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(reader, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!reader.read(&mRigidBody->mShapeType))
		{
			LogError("FileReader::read() failed for 'shape type'");
			return false;
		}

		if (!reader.read(&mRigidBody->mMass))
		{
			LogError("FileReader::read() failed for 'mass'");
			return false;
		}

		if (!reader.read(&mRigidBody->mKinematic))
		{
			LogError("FileReader::read() failed for 'kinematic'");
			return false;
		}

		return true;
	}

	bool RigidBodySerializer::write(FileWriter& writer)
	{
		if (!ComponentSerializer::write(writer))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		if (!writer.write(&mRigidBody->mShapeType))
		{
			LogError("FileWriter::write() failed for 'shape type'");
			return false;
		}

		if (!writer.write(&mRigidBody->mMass))
		{
			LogError("FileWriter::write() failed for 'mass'");
			return false;
		}

		if (!writer.write(&mRigidBody->mKinematic))
		{
			LogError("FileWriter::write() failed for 'kinematic'");
			return false;
		}

		return true;		
	}

	bool RigidBodySerializer::read(json& object, ResourceCache& cache)
	{
		if (!ComponentSerializer::read(object, cache))
		{
			LogError("ComponentSerialzer::read() failed");
			return false;
		}

		if (!object.contains("shapeType"))
		{
			LogError("JSON RigidBody object doesn't contain 'shapeType' key");
			return false;
		}

		if (!object.contains("mass"))
		{
			LogError("JSON RigidBody object doesn't contain 'shapeType' key");
			return false;
		}

		if (!object.contains("kinematic"))
		{
			LogError("JSON RigidBody object doesn't contain 'shapeType' key");
			return false;
		}

		mRigidBody->mShapeType = (RigidShapeType)object["shapeType"].get<uint32_t>();
		mRigidBody->mMass = object["mass"].get<float>();
		mRigidBody->mKinematic = object["kinematic"].get<bool>();

		return true;
	}

	bool RigidBodySerializer::write(json& object)
	{
		if (!ComponentSerializer::write(object))
		{
			LogError("ComponentSerialzer::write() failed");
			return false;
		}

		object["shapeType"] = (uint32_t)mRigidBody->mShapeType;
		object["mass"] = mRigidBody->mMass;
		object["kinematic"] = mRigidBody->mKinematic;

		return true;
	}
}