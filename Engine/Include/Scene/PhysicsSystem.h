#pragma once

#include "Core/Singleton.h"
#include <memory>
#include <string>
#include <typeindex>

namespace Trinity
{
	class Physics;
	class Scene;

	class PhysicsSystem : public Singleton<PhysicsSystem>
	{
	public:

		PhysicsSystem();
		virtual ~PhysicsSystem() = default;

		PhysicsSystem(const PhysicsSystem&) = delete;
		PhysicsSystem& operator = (const PhysicsSystem&) = delete;

		PhysicsSystem(PhysicsSystem&&) = default;
		PhysicsSystem& operator = (PhysicsSystem&&) = default;

		Physics* getPhysics() const
		{
			return mPhysics.get();
		}

		virtual void setScene(Scene& scene);
		virtual void update(float deltaTime);

	protected:

		std::unique_ptr<Physics> mPhysics{ nullptr };
		Scene* mScene{ nullptr };
	};
}