#include "Scene/SceneSystem.h"
#include "Scene/Scene.h"
#include "Scene/Sprite.h"
#include "Scene/QuadTree.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Components/TextureRenderable.h"
#include "Scene/Components/RigidBody.h"
#include "Scene/Components/Collider.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/RenderTarget.h"
#include "Physics/Physics.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool SceneSystem::create(RenderTarget& renderTarget, ResourceCache& cache)
	{
		mPhysics = std::make_unique<Physics>();
		mRenderer = std::make_unique<BatchRenderer>();

		if (!mRenderer->create(renderTarget, cache, kTexturedShader, kColoredShader))
		{
			LogError("BatchRenderer::create() failed with shader: '%s' and '%s'", kTexturedShader, kColoredShader);
			return true;
		}

		return true;
	}

	void SceneSystem::destroy()
	{
		mScene = nullptr;
		mCamera = nullptr;
		mRenderer = nullptr;
	}

	void SceneSystem::setScene(Scene& scene)
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

	void SceneSystem::setCamera(Camera& camera)
	{
		mCamera = &camera;
	}

	void SceneSystem::setCamera(const std::string& cameraNodeName)
	{
		auto cameraNode = mScene->findNode(cameraNodeName);
		if (cameraNode != nullptr)
		{
			mCamera = &cameraNode->getComponent<Camera>();
		}
	}

	void SceneSystem::setupQuadTree(const BoundingRect& sceneBounds, const BoundingRect& minBounds)
	{
		mQuadTree = std::make_unique<QuadTree>();
		mQuadTree->create(minBounds, sceneBounds);
	}

	void SceneSystem::update(float deltaTime)
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

		auto& physics = Physics::get();

		for (uint32_t idx = 0; idx < physics.getNumRelaxations(); idx++)
		{
			for (auto* collider : colliders)
			{
				updateQuadTree(*collider);
			}

			for (auto* collider : colliders)
			{
				collision(*collider);
			}
		}
	}

	void SceneSystem::draw(const RenderPass& renderPass)
	{
		if (mCamera != nullptr)
		{
			auto viewProj = mCamera->getProjection() * mCamera->getView();
			draw(renderPass, viewProj);
		}
	}

	void SceneSystem::draw(const RenderPass& renderPass, const glm::mat4& viewProj)
	{
		if (mScene != nullptr)
		{
			drawTextures(renderPass, viewProj);
			drawSprites(renderPass, viewProj);
		}
	}

	void SceneSystem::updateQuadTree(Collider& collider)
	{
		if (mQuadTree != nullptr)
		{
			collider.update();
			mQuadTree->update(collider.getQuadTreeData());
		}
	}

	void SceneSystem::queryColliders(Collider& collider, std::vector<Collider*>& colliders)
	{
		if (mQuadTree != nullptr)
		{
			std::vector<QuadTreeData*> result;
			mQuadTree->query(collider.getQuadTreeData().bounds, result);

			for (auto* data : result)
			{
				if (data != nullptr)
				{	
					auto* other = ((ColliderData*)data)->collider;
					if (other == &collider)
					{
						continue;
					}

					for (uint32_t idx = 0; idx < mScene->getNumLayers(); idx++)
					{
						if (other->hasLayer(idx) && collider.hasLayer(idx))
						{
							colliders.push_back(other);
							break;
						}
					}
				}
			}
		}
		else
		{
			auto others = mScene->getComponents<Collider>();
			for (auto& other : others)
			{
				if (other == &collider)
				{
					continue;
				}

				auto* rb1 = collider.getRigidBody();
				auto* rb2 = other->getRigidBody();

				for (uint32_t idx = 0; idx < mScene->getNumLayers(); idx++)
				{
					if (collider.hasLayer(idx) && other->hasLayer(idx))
					{
						if (rb1->getBounds().collideStatus(rb2->getBounds()) != BoundCollideStatus::Outside)
						{
							colliders.push_back(other);	
							break;
						}
					}
				}
			}
		}
	}	

	void SceneSystem::collision(Collider& collider)
	{
		std::vector<Collider*> others;
		queryColliders(collider, others);

		auto* rb1 = collider.getRigidBody();
		auto* rs1 = rb1->getShape();

		std::vector<Collider*> colliders;
		for (auto* other : others)
		{
			CollisionInfo collisionInfo{};
			auto* rb2 = other->getRigidBody();
			auto* rs2 = rb2->getShape();

			if (Physics::get().collision(*rs1, *rs2, collisionInfo))
			{
				colliders.push_back(other);
				if (!rb1->isKinematic() && !rb2->isKinematic())
				{
					Physics::get().resolve(*rs1, *rs2, collisionInfo);
				}
			}
		}

		collider.setColliders(std::move(colliders));
	}

	void SceneSystem::drawTextures(const RenderPass& renderPass, const glm::mat4& viewProj)
	{
		auto renderables = mScene->getComponents<TextureRenderable>();
		mRenderer->begin(viewProj);

		for (auto& renderable : renderables)
		{
			if (!renderable->isActive())
			{
				continue;
			}

			auto* texture = renderable->getTexture();
			auto& transform = renderable->getNode()->getTransform();
			auto& flip = renderable->getFlip();

			if (texture != nullptr)
			{
				mRenderer->drawTexture(
					texture,
					glm::vec2{ 0.0f, 0.0f },
					glm::vec2{ texture->getWidth(), texture->getHeight() },
					renderable->getOrigin(),
					transform.getWorldMatrix(),
					renderable->getColor(),
					flip.x,
					flip.y
				);
			}
		}

		mRenderer->end(renderPass);
	}

	void SceneSystem::drawSprites(const RenderPass& renderPass, const glm::mat4& viewProj)
	{
		auto renderables = mScene->getComponents<SpriteRenderable>();
		std::sort(renderables.begin(), renderables.end(), 
			[](const auto& a, const auto& b) {
				return a->getLayer() > b->getLayer();
			}
		);

		mRenderer->begin(viewProj);

		for (auto& renderable : renderables)
		{
			auto* sprite = renderable->getSprite();
			auto& transform = renderable->getNode()->getTransform();
			auto& flip = renderable->getFlip();

			sprite->draw(
				*mRenderer, 
				renderable->getActiveFrameIndex(), 
				renderable->getOrigin(), 
				transform.getWorldMatrix(), 
				renderable->getColor(), 
				flip.x, 
				flip.y
			);
		}

		mRenderer->end(renderPass);
	}
}