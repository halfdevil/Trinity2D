#include "Scene/PhysicsSystem.h"
#include "Scene/Components/RigidBody.h"
#include "Scene/Components/Collider.h"
#include "Scene/Scene.h"
#include "Physics/Physics.h"

namespace Trinity
{	
	PhysicsSystem::PhysicsSystem() :
		mPhysics(std::make_unique<Physics>())
	{	
	}

	void PhysicsSystem::setScene(Scene& scene)
	{
		mScene = &scene;

		auto rigidBodies = mScene->getComponents<RigidBody>();
		auto colliders = mScene->getComponents<Collider>();

		for (auto* rigidBody : rigidBodies)
		{
			rigidBody->init();
		}

		for (auto* collider : colliders)
		{
			collider->init();
		}
	}

	void PhysicsSystem::update(float deltaTime)
	{
		auto rigidBodies = mScene->getComponents<RigidBody>();
		auto colliders = mScene->getComponents<Collider>();

		for (auto* rigidBody : rigidBodies)
		{
			if (!rigidBody->isKinematic())
			{
				rigidBody->update(deltaTime);
			}
		}

		for (auto* rigidBody : rigidBodies)
		{
			if (!rigidBody->isKinematic())
			{
				rigidBody->updateTransform();
			}
		}
	}
}