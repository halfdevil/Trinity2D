#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/ScriptContainer.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/TextureRenderable.h"
#include "Scene/Components/Scripts/CameraController.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "VFS/FileSystem.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include <queue>

namespace Trinity
{
	bool Scene::create(const std::string& fileName)
	{
		mComponentFactory = std::make_unique<ComponentFactory>();
		registerDefaultComponents();
		
		return true;
	}

	void Scene::destroy()
	{
		mNodes.clear();
		mComponents.clear();
	}

	std::type_index Scene::getType() const
	{
		return typeid(Scene);
	}

	void Scene::registerDefaultComponents()
	{
		mComponentFactory->registerCreator<Light>();
	}

	bool Scene::hasComponent(const std::type_index& type) const
	{
		auto it = mComponents.find(type);
		return (it != mComponents.end() && !it->second.empty());
	}

	Node* Scene::findNode(const std::string& name)
	{
		for (auto rootNode : mRoot->getChildren())
		{
			std::queue<Node*> traverseNodes;
			traverseNodes.push(rootNode);

			while (!traverseNodes.empty())
			{
				auto node = traverseNodes.front();
				traverseNodes.pop();

				if (node->getName() == name)
				{
					return node;
				}

				for (auto childNode : node->getChildren())
				{
					traverseNodes.push(childNode);
				}
			}
		}

		return nullptr;
	}

	Node* Scene::getNode(uint32_t idx) const
	{
		if (idx < (uint32_t)mNodes.size())
		{
			return mNodes[idx].get();
		}

		return nullptr;
	}

	const std::vector<std::unique_ptr<Component>>& Scene::getComponents(const std::type_index& type) const
	{
		return mComponents.at(type);
	}

	void Scene::addNode(std::unique_ptr<Node> node)
	{
		mNodes.emplace_back(std::move(node));
	}

	void Scene::addChild(Node& child)
	{
		if (mRoot != nullptr)
		{
			mRoot->addChild(child);
		}
	}

	void Scene::setNodes(std::vector<std::unique_ptr<Node>> nodes)
	{
		mNodes = std::move(nodes);
	}

	void Scene::setRoot(Node& node)
	{
		mRoot = &node;
	}

	void Scene::addComponent(std::unique_ptr<Component> component)
	{
		mComponents[component->getType()].push_back(std::move(component));
	}

	void Scene::addComponent(std::unique_ptr<Component> component, Node& node)
	{
		node.setComponent(*component);
		addComponent(std::move(component));
	}

	void Scene::setComponents(const std::type_index& type, std::vector<std::unique_ptr<Component>> components)
	{
		mComponents[type] = std::move(components);
	}

	Light* Scene::addLight(LightType type, const glm::vec3& position, const glm::vec3& rotation,
		const LightProperties& properties, Node* parent)
	{
		auto lightPtr = std::make_unique<Light>();
		auto lightNode = std::make_unique<Node>();

		if (parent != nullptr)
		{
			lightNode->setParent(*parent);
		}

		lightPtr->setName("light");
		lightNode->setName("lightNode");

		lightPtr->setNode(*lightNode);
		lightPtr->setLightType(type);
		lightPtr->setLightProperties(properties);

		auto& transform = lightNode->getTransform();
		transform.setTranslation(position);
		transform.setRotation(rotation);

		auto* light = lightPtr.get();
		lightNode->setComponent(*light);

		addChild(*lightNode);
		addComponent(std::move(lightPtr), *lightNode);
		addNode(std::move(lightNode));

		return light;
	}

	Light* Scene::addPointLight(const glm::vec3& position, const LightProperties& properties,
		Node* parent)
	{
		return addLight(LightType::Point, position, {}, properties, parent);
	}

	Light* Scene::addDirectionalLight(const glm::vec3& rotation, const LightProperties& properties,
		Node* parent)
	{
		return addLight(LightType::Directional, {}, rotation, properties, parent);
	}

	Light* Scene::addSpotLight(const glm::vec3& position, const glm::vec3& rotation,
		const LightProperties& properties, Node* parent)
	{
		return addLight(LightType::Spot, position, rotation, properties, parent);
	}
	
	TextureRenderable* Scene::addTextureRenderable(
		Texture& texture, 
		const std::string& nodeName, 
		const glm::vec2& origin,
		const glm::bvec2& flip, 
		const glm::vec4& color, 
		const glm::vec3& position, 
		const glm::vec3& rotation, 
		const glm::vec3& scale
	)
	{
		auto node = findNode(nodeName);
		if (!node)
		{
			LogError("Node not found: '%s'", nodeName.c_str());
			return nullptr;
		}

		auto renderable = std::make_unique<TextureRenderable>();
		renderable->setTexture(texture);
		renderable->setOrigin(origin);
		renderable->setFlip(flip);
		renderable->setNode(*node);

		auto& transform = node->getTransform();
		transform.setTranslation(position);
		transform.setRotation(rotation);
		transform.setScale(scale);

		auto* renderablePtr = renderable.get();
		node->setComponent(*renderable);
		addComponent(std::move(renderable), *node);

		return renderablePtr;
	}

	Camera* Scene::addCamera(const std::string& nodeName, const glm::vec2& size, float nearPlane, float farPlane, 
		const glm::vec3& position, const glm::vec3& rotation, Node* parent)
	{
		auto cameraPtr = std::make_unique<Camera>();
		auto cameraNode = std::make_unique<Node>();

		if (parent != nullptr)
		{
			cameraNode->setParent(*parent);
		}

		cameraPtr->setName("camera");
		cameraNode->setName(nodeName);

		cameraPtr->setNode(*cameraNode);
		cameraPtr->setSize(size);
		cameraPtr->setNearPlane(nearPlane);
		cameraPtr->setFarPlane(farPlane);

		auto& transform = cameraNode->getTransform();
		transform.setTranslation(position);
		transform.setRotation(rotation);

		auto* camera = cameraPtr.get();
		cameraNode->setComponent(*camera);

		addChild(*cameraNode);
		addComponent(std::move(cameraPtr), *cameraNode);
		addNode(std::move(cameraNode));

		return camera;
	}

	CameraController* Scene::addCameraController(const std::string& nodeName)
	{
		auto cameraNode = findNode(nodeName);
		if (!cameraNode)
		{
			LogWarning("Camera node '%s' not found. Looking for 'default_camera' node", nodeName.c_str());
			cameraNode = findNode("default_camera");
		}

		if (!cameraNode)
		{
			LogError("Default camera node 'default_camera' not found.");
			return nullptr;
		}

		auto controller = std::make_unique<CameraController>();
		controller->setNode(*cameraNode);

		auto* controllerPtr = controller.get();
		addComponent(std::move(controller));

		auto& container = cameraNode->getScriptContainer();
		container.setScript(*controllerPtr);

		return controllerPtr;
	}
}