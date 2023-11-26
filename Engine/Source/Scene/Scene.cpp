#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/Component.h"
#include "Scene/Components/Light.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/ScriptContainer.h"
#include "Scene/Components/TextureRenderable.h"
#include "Scene/Components/SpriteRenderable.h"
#include "Scene/Components/Scripts/CameraController.h"
#include "Scene/Components/Scripts/SpriteAnimator.h"
#include "Scene/ComponentFactory.h"
#include "Graphics/Texture.h"
#include "VFS/FileSystem.h"
#include "Core/ResourceCache.h"
#include "Core/Logger.h"
#include <queue>

namespace Trinity
{
	Scene::Scene()
		: mComponentFactory(std::make_unique<ComponentFactory>())
	{
		registerDefaultComponents();
	}

	std::type_index Scene::getType() const
	{
		return typeid(Scene);
	}

	void Scene::registerDefaultComponents()
	{
		mComponentFactory->registerCreator<Camera>("Camera");
		mComponentFactory->registerCreator<Light>("Light");
		mComponentFactory->registerCreator<SpriteRenderable>("Sprite Renderable");
		mComponentFactory->registerCreator<SpriteAnimator>("Sprite Animator");
		mComponentFactory->registerCreator<TextureRenderable>("Texture Renderable");
	}

	void Scene::clear()
	{
		mNodes.clear();
		mComponents.clear();
		mRoot = nullptr;
	}

	ISerializer* Scene::getSerializer()
	{
		static SceneSerializer serializer;
		serializer.setScene(*this);

		return &serializer;
	}

	bool Scene::hasComponent(const std::type_index& type) const
	{
		auto it = mComponents.find(type);
		return (it != mComponents.end() && !it->second.empty());
	}

	Node* Scene::findNode(const std::string& name) const
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

	Node* Scene::findNode(const UUIDv4::UUID& uuid) const
	{
		if (auto it = mNodeMap.find(uuid); it != mNodeMap.end())
		{
			return it->second;
		}

		return nullptr;
	}

	const std::vector<std::unique_ptr<Component>>& Scene::getComponents(const std::type_index& type) const
	{
		return mComponents.at(type);
	}

	void Scene::addNode(std::unique_ptr<Node> node)
	{
		mNodeMap.insert(std::make_pair(node->getUUID(), node.get()));
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
		for (auto& node : nodes)
		{
			mNodeMap.insert(std::make_pair(node->getUUID(), node.get()));
		}

		mNodes = std::move(nodes);
	}

	void Scene::setRoot(Node& node)
	{
		mRoot = &node;
	}

	void Scene::updateNodeUUID(Node* node, const UUIDv4::UUID& newUUID)
	{
		if (auto it = mNodeMap.find(node->getUUID()); it != mNodeMap.end())
		{
			mNodeMap.erase(it);
		}

		node->setUUID(newUUID);
		mNodeMap.insert(std::make_pair(newUUID, node));
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

	Node* Scene::addEmpty(const glm::vec3& position, const glm::vec3& rotation, Node* parent)
	{
		auto node = std::make_unique<Node>();
		node->setName("Node");

		if (parent != nullptr)
		{
			parent->addChild(*node);
		}
		else
		{
			addChild(*node);
		}

		auto* nodePtr = node.get();
		addNode(std::move(node));

		return nodePtr;
	}

	Light* Scene::addLight(const std::string& nodeName, LightType type, const LightProperties& properties)
	{
		auto node = findNode(nodeName);
		if (!node)
		{
			LogError("Node not found: '%s'", nodeName.c_str());
			return nullptr;
		}

		auto light = std::make_unique<Light>();		
		light->setName("Light");
		light->setNode(*node);
		light->setLightType(type);
		light->setLightProperties(properties);

		auto* lightPtr = light.get();
		addComponent(std::move(light), *node);

		return lightPtr;
	}

	Light* Scene::addPointLight(const std::string& nodeName, const LightProperties& properties)
	{
		return addLight(nodeName, LightType::Point, properties);
	}

	Light* Scene::addDirectionalLight(const std::string& nodeName, const LightProperties& properties)
	{
		return addLight(nodeName, LightType::Directional, properties);
	}

	Light* Scene::addSpotLight(const std::string& nodeName, const LightProperties& properties)
	{
		return addLight(nodeName, LightType::Spot, properties);
	}
	
	TextureRenderable* Scene::addTextureRenderable(
		Texture& texture, 
		const std::string& nodeName, 
		const glm::vec2& origin,
		const glm::bvec2& flip, 
		const glm::vec4& color
	)
	{
		auto node = findNode(nodeName);
		if (!node)
		{
			LogError("Node not found: '%s'", nodeName.c_str());
			return nullptr;
		}

		auto renderable = std::make_unique<TextureRenderable>();
		renderable->setName("TextureRenderable");
		renderable->setNode(*node);
		renderable->setTexture(texture);
		renderable->setOrigin(origin);
		renderable->setFlip(flip);

		auto* renderablePtr = renderable.get();
		addComponent(std::move(renderable), *node);

		return renderablePtr;
	}

	Camera* Scene::addCamera(const std::string& nodeName, const glm::vec2& size, float nearPlane, float farPlane)
	{
		auto node = findNode(nodeName);
		if (!node)
		{
			LogError("Node not found: '%s'", nodeName.c_str());
			return nullptr;
		}

		auto camera = std::make_unique<Camera>();
		camera->setName("Camera");
		camera->setNode(*node);
		camera->setSize(size);
		camera->setNearPlane(nearPlane);
		camera->setFarPlane(farPlane);

		auto* cameraPtr = camera.get();
		addComponent(std::move(camera), *node);
		
		return cameraPtr;
	}

	CameraController* Scene::addCameraController(const std::string& nodeName)
	{
		auto node = findNode(nodeName);
		if (!node)
		{
			LogError("Node not found: '%s'", nodeName.c_str());
			return nullptr;
		}

		auto controller = std::make_unique<CameraController>();
		controller->setName("CameraController");
		controller->setNode(*node);

		auto* controllerPtr = controller.get();
		addComponent(std::move(controller));

		auto& container = node->getScriptContainer();
		container.setScript(*controllerPtr);

		return controllerPtr;
	}

	void SceneSerializer::setScene(Scene& scene)
	{
		mScene = &scene;
	}

	bool SceneSerializer::read(FileReader& reader, ResourceCache& cache)
	{
		uint32_t numNodes{ 0 };
		if (!reader.read(&numNodes))
		{
			LogError("FileReader::read() failed for Scene 'num nodes'");
			return false;
		}

		for (uint32_t idx = 0; idx < numNodes; idx++)
		{
			auto node = std::make_unique<Node>();
			if (auto* serializer = node->getSerializer(*mScene); serializer != nullptr)
			{
				if (!serializer->read(reader, cache))
				{
					LogError("NodeSerializer::read() failed for node: '%d'", idx);
					return false;
				}
			}

			mScene->addNode(std::move(node));
		}

		uint32_t numComponents{ 0 };
		if (!reader.read(&numComponents))
		{
			LogError("FileReader::read() failed for Scene 'num components'");
			return false;
		}

		for (uint32_t idx = 0; idx < numComponents; idx++)
		{
			auto uuid = UUIDv4::UUID::fromStrFactory(reader.readString());
			if (!mScene->mComponentFactory->hasRegister(uuid))
			{
				LogError("Component not register for uuid: '%s'", uuid.str().c_str());
				return false;
			}

			auto component = mScene->mComponentFactory->createComponent(uuid);
			if (component != nullptr)
			{
				if (auto* serializer = component->getSerializer(*mScene); serializer != nullptr)
				{
					if (!serializer->read(reader, cache))
					{
						LogError("ComponentSerializer::read() failed for uuid: '%s'", uuid.str().c_str());
						return false;
					}
				}

				mScene->addComponent(std::move(component));
			}
		}

		auto rootUUID = UUIDv4::UUID::fromStrFactory(reader.readString());
		mScene->mRoot = mScene->findNode(rootUUID);

		return true;
	}

	bool SceneSerializer::write(FileWriter& writer)
	{
		const uint32_t numNodes = (uint32_t)mScene->mNodes.size();
		if (!writer.write(&numNodes))
		{
			LogError("FileWriter::write() failed for 'num nodes' for scene: '%s'", mScene->mName.c_str());
			return false;
		}

		for (auto& node : mScene->mNodes)
		{
			if (auto* serializer = node->getSerializer(*mScene); serializer != nullptr)
			{
				if (!serializer->write(writer))
				{
					LogError("NodeSerializer::write() failed for node: '%s'", node->getName().c_str());
					return false;
				}
			}
		}

		const uint32_t numComponents = (uint32_t)mScene->mComponents.size();
		if (!writer.write(&numComponents))
		{
			LogError("FileWriter::write() failed for 'num components' for scene: '%s'", mScene->mName.c_str());
			return false;
		}

		for (auto& it : mScene->mComponents)
		{
			auto& components = it.second;
			for (auto& component : components)
			{
				if (auto* serializer = component->getSerializer(*mScene); serializer != nullptr)
				{
					if (!serializer->write(writer))
					{
						LogError("ComponentSerialzer::write() failed for component: '%s'", component->getName().c_str());
						return false;
					}
				}
			}
		}

		std::string rootUUID;
		if (mScene->mRoot != nullptr)
		{
			rootUUID = mScene->mRoot->getUUID().str();
		}

		if (!writer.writeString(rootUUID))
		{
			LogError("FileWriter::write() failed for 'root uuid' for scene: '%s'", mScene->mName.c_str());
			return false;
		}

		return true;
	}

	bool SceneSerializer::read(json& object, ResourceCache& cache)
	{
		if (!object.contains("nodes"))
		{
			LogError("Scene JSON object doesn't contain 'nodes' key");
			return false;
		}

		if (!object.contains("components"))
		{
			LogError("Scene JSON object doesn't contain 'components' key");
			return false;
		}

		if (!object.contains("root"))
		{
			LogError("Scene JSON object doesn't contain 'root' key");
			return false;
		}

		for (auto nodeJson : object["nodes"])
		{
			auto node = std::make_unique<Node>();
			auto* nodePtr = node.get();

			mScene->addNode(std::move(node));

			if (auto* serializer = nodePtr->getSerializer(*mScene); serializer != nullptr)
			{
				if (!serializer->read(nodeJson, cache))
				{
					LogError("NodeSerializer::read() failed for node for scene: '%s'", mScene->mName.c_str());
					return false;
				}
			}
		}

		for (auto componentJson : object["components"])
		{
			if (!componentJson.contains("uuid"))
			{
				LogError("Component JSON object doesn't contain 'uuid' key");
				return false;
			}

			auto uuid = UUIDv4::UUID::fromStrFactory(componentJson["uuid"].get<std::string>());
			if (!mScene->mComponentFactory->hasRegister(uuid))
			{
				LogError("Component not register for uuid: '%s'", uuid.str().c_str());
				return false;
			}

			auto component = mScene->mComponentFactory->createComponent(uuid);
			auto* componentPtr = component.get();

			mScene->addComponent(std::move(component));

			if (componentPtr != nullptr)
			{
				if (auto* serializer = componentPtr->getSerializer(*mScene); serializer != nullptr)
				{
					if (!serializer->read(componentJson, cache))
					{
						LogError("ComponentSerializer::read() failed for uuid: '%s'", uuid.str().c_str());
						return false;
					}
				}
			}
		}

		auto rootUUID = UUIDv4::UUID::fromStrFactory(object["root"].get<std::string>());
		mScene->mRoot = mScene->findNode(rootUUID);

		return true;
	}

	bool SceneSerializer::write(json& object)
	{
		std::vector<json> nodesJson;
		std::vector<json> componentsJson;

		for (auto& node : mScene->mNodes)
		{
			if (auto* serializer = node->getSerializer(*mScene); serializer != nullptr)
			{
				json nodeJson;
				if (!serializer->write(nodeJson))
				{
					LogError("NodeSerializer::write() failed for node: '%s'", node->getName().c_str());
					return false;
				}

				nodesJson.push_back(std::move(nodeJson));
			}
		}

		for (auto& it : mScene->mComponents)
		{
			auto& components = it.second;
			for (auto& component : components)
			{
				if (auto* serializer = component->getSerializer(*mScene); serializer != nullptr)
				{
					json componentJson;
					if (!serializer->write(componentJson))
					{
						LogError("ComponentSerialzer::write() failed for component: '%s'", component->getName().c_str());
						return false;
					}

					componentsJson.push_back(std::move(componentJson));
				}
			}
		}

		std::string rootUUID;
		if (mScene->mRoot != nullptr)
		{
			rootUUID = mScene->mRoot->getUUID().str();
		}

		object["nodes"] = std::move(nodesJson);
		object["components"] = std::move(componentsJson);
		object["root"] = rootUUID;

		return true;
	}
}