#include "Widgets/SceneViewport.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/SceneSystem.h"
#include "Scene/Components/Transform.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Physics/Physics.h"
#include "Graphics/Texture.h"
#include "Graphics/BatchRenderer.h"
#include "Graphics/GraphicsDevice.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/RenderPass.h"
#include "Graphics/LineCanvas.h"
#include "Core/EditorTheme.h"
#include "Core/EditorResources.h"
#include "Core/EditorGizmo.h"
#include "Core/EditorCamera.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"

namespace Trinity
{
	bool SceneViewport::create(EditorResources& resources)
	{
		if (!Viewport::create(resources))
		{
			LogError("Viewport::create() failed");
			return false;
		}

		mSceneSystem = std::make_unique<SceneSystem>();
		if (!mSceneSystem->create(*mFrameBuffer, *resources.getResourceCache()))
		{
			LogError("SceneSystem::create() failed");
			return false;
		}

		return true;
	}

	void SceneViewport::destroy()
	{
		Viewport::destroy();
		mSceneSystem = nullptr;
	}

	void SceneViewport::setScene(Scene& scene)
	{
		mScene = &scene;
		mSelectedNode = mScene->getRoot();
		mSceneSystem->setScene(scene);
	}

	void SceneViewport::setSelectedNode(Node* node)
	{
		mSelectedNode = node;
	}

	void SceneViewport::drawContent(float deltaTime)
	{
		mRenderPass->begin(*mFrameBuffer);
		
		if (mScene != nullptr)
		{
			mSceneSystem->draw(*mRenderPass, mCamera->getViewProj());
		}

		if (mLineCanvas != nullptr)
		{
			mLineCanvas->draw(mCamera->getViewProj(), *mRenderPass);
		}

		mRenderPass->end();
	}

	void SceneViewport::editTransform(float x, float y, float width, float height)
	{
		if (mSelectedNode != nullptr)
		{
			mGizmo->setRect(x, y, width, height);

			auto& transform = mSelectedNode->getTransform();
			auto matrix = transform.getWorldMatrix();

			if (mGizmo->show(mCamera->getView(), mCamera->getProjection(), matrix))
			{
				transform.setWorldMatrix(matrix);
			}
		}
	}

	void SceneViewport::onViewportResize(uint32_t width, uint32_t height)
	{
		Viewport::onViewportResize(width, height);

		if (mCamera != nullptr)
		{
			float halfWidth = 0.5f * width;
			float halfHeight = 0.5f * height;

			mCamera->setSize(-halfWidth, halfWidth, -halfHeight, halfHeight);
		}
	}
}