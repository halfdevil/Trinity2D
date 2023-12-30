#pragma once

#include "Core/Resource.h"
#include <glm/glm.hpp>
#include "Scene/Component.h"
#include "Scene/Node.h"
#include "Scene/Components/Light.h"
#include "VFS/Serializer.h"
#include <algorithm>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Trinity
{
	class SceneSerializer;
	class ResourceCache;
	class ComponentFactory;
	class Camera;
	class CameraController;
	class TextureRenderable;
	class Texture;
	class SpriteRenderable;
	class SpriteAnimator;
	class Sprite;

	class Scene : public Resource
	{
	public:

		friend class SceneSerializer;

		Scene();
		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator = (const Scene&) = delete;

		Scene(Scene&&) = default;
		Scene& operator = (Scene&&) = default;

		Node* getRoot() const
		{
			return mRoot;
		}

		uint32_t getNumLayers() const
		{
			return mNumLayers;
		}

		ComponentFactory* getComponentFactory()
		{
			return mComponentFactory.get();
		}

		virtual std::type_index getType() const override;
		virtual void registerDefaultComponents();
		virtual void clear();

		virtual ISerializer* getSerializer();

		virtual bool hasComponent(const std::type_index& type) const;
		virtual Node* findNode(const std::string& name) const;
		virtual Node* findNode(const UUIDv4::UUID& uuid) const;

		virtual const std::vector<std::unique_ptr<Component>>& getComponents(const std::type_index& type) const;
		virtual Component* getComponent(const std::type_index& type) const;

		virtual void setNumLayers(uint32_t numLayers);
		virtual void addNode(std::unique_ptr<Node> node);
		virtual void addChild(Node& child);
		virtual void setNodes(std::vector<std::unique_ptr<Node>> nodes);
		virtual void setRoot(Node& node);
		virtual void updateNodeUUID(Node* node, const UUIDv4::UUID& newUUID);

		virtual void addComponent(std::unique_ptr<Component> component);
		virtual void addComponent(std::unique_ptr<Component> component, Node& node);
		virtual void setComponents(const std::type_index& type, std::vector<std::unique_ptr<Component>> components);

		virtual Node* addEmpty(
			const glm::vec3& position = glm::vec3{ 0.0f },
			const glm::vec3& rotation = glm::vec3{ 0.0f },
			Node* parent = nullptr
		);

		virtual Light* addLight(
			const std::string& nodeName,
			LightType type,
			const LightProperties& properties = {}
		);

		virtual Light* addPointLight(
			const std::string& nodeName,
			const LightProperties& properties = {}
		);

		virtual Light* addDirectionalLight(
			const std::string& nodeName,
			const LightProperties& properties = {}
		);

		virtual Light* addSpotLight(
			const std::string& nodeName,
			const LightProperties& properties = {}
		);

		virtual TextureRenderable* addTextureRenderable(
			Texture& texture,
			const std::string& nodeName,
			const glm::vec2& origin = glm::vec2{ 0.5f },
			const glm::bvec2& flip = glm::bvec2{ false },
			const glm::vec4& color = glm::vec4{ 0.0f }
		);

		virtual Camera* addCamera(
			const std::string& nodeName,
			const glm::vec2& size
		);

		virtual CameraController* addCameraController(
			const std::string& nodeName
		);

		virtual SpriteRenderable* addSpriteRenderable(
			Sprite& sprite,
			const std::string& nodeName,
			const glm::vec2& origin = glm::vec2{ 0.5f },
			const glm::bvec2& flip = glm::bvec2{ false },
			const glm::vec4& color = glm::vec4{ 0.0f }
		);

		virtual SpriteAnimator* addSpriteAnimator(
			const std::string& nodeName
		);

	public:

		template <typename T>
		void setComponents(std::vector<std::unique_ptr<T>> components)
		{
			std::vector<std::unique_ptr<Component>> result(components.size());
			std::transform(components.begin(), components.end(), result.begin(),
				[](auto& component) {
					return std::unique_ptr<Component>(std::move(component));
				}
			);

			setComponents(typeid(T), std::move(result));
		}

		template <typename T>
		void clearComponents()
		{
			setComponents(typeid(T), {});
		}

		template <typename T>
		std::vector<T*> getComponents() const
		{
			std::vector<T*> result;

			if (hasComponent(typeid(T)))
			{
				auto& sceneComponents = getComponents(typeid(T));

				result.resize(sceneComponents.size());
				std::transform(sceneComponents.begin(), sceneComponents.end(), result.begin(),
					[](const auto& component) -> T* {
						return dynamic_cast<T*>(component.get());
					}
				);
			}

			return result;
		}

		template <typename T>
		T* getComponent() const
		{
			return (T*)getComponent(typeid(T));
		}

		template <typename T>
		bool hasComponent() const
		{
			return hasComponent(typeid(T));
		}

	protected:

		Node* mRoot{ nullptr };
		uint32_t mNumLayers{ 0 };
		std::unique_ptr<ComponentFactory> mComponentFactory{ nullptr };
		std::vector<std::unique_ptr<Node>> mNodes;
		std::unordered_map<UUIDv4::UUID, Node*> mNodeMap;
		std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> mComponents;
	};

	class SceneSerializer : public ISerializer
	{
	public:

		SceneSerializer() = default;
		virtual ~SceneSerializer() = default;

		SceneSerializer(const SceneSerializer&) = delete;
		SceneSerializer& operator = (const SceneSerializer&) = delete;

		SceneSerializer(SceneSerializer&&) = default;
		SceneSerializer& operator = (SceneSerializer&&) = default;

		virtual void setScene(Scene& scene);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Scene* mScene{ nullptr };
	};
}