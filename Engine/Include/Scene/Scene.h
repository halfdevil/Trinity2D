#pragma once

#include "Core/Resource.h"
#include <glm/glm.hpp>
#include "Scene/Component.h"
#include "Scene/Node.h"
#include "Scene/Components/Light.h"
#include <algorithm>
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace Trinity
{
	class ResourceCache;
	class ComponentFactory;
	class Camera;
	class CameraController;
	class TextureRenderable;
	class Texture;

	class Scene : public Resource
	{
	public:

		Scene() = default;
		virtual ~Scene() = default;

		Scene(const Scene&) = delete;
		Scene& operator = (const Scene&) = delete;

		Scene(Scene&&) = default;
		Scene& operator = (Scene&&) = default;

		Node* getRoot() const
		{
			return mRoot;
		}

		ComponentFactory* getComponentFactory()
		{
			return mComponentFactory.get();
		}

		virtual bool create(const std::string& fileName);
		virtual void destroy();

		virtual std::type_index getType() const override;
		virtual void registerDefaultComponents();

		virtual bool hasComponent(const std::type_index& type) const;
		virtual Node* findNode(const std::string& name);
		virtual Node* getNode(uint32_t idx) const;
		virtual const std::vector<std::unique_ptr<Component>>& getComponents(const std::type_index& type) const;

		virtual void addNode(std::unique_ptr<Node> node);
		virtual void addChild(Node& child);
		virtual void setNodes(std::vector<std::unique_ptr<Node>> nodes);
		virtual void setRoot(Node& node);

		virtual void addComponent(std::unique_ptr<Component> component);
		virtual void addComponent(std::unique_ptr<Component> component, Node& node);
		virtual void setComponents(const std::type_index& type, std::vector<std::unique_ptr<Component>> components);

		virtual Light* addLight(
			LightType type, 
			const glm::vec3& position, 
			const glm::vec3& rotation = {},
			const LightProperties& properties = {}, 
			Node* parent = nullptr
		);

		virtual Light* addPointLight(
			const glm::vec3& position, 
			const LightProperties& properties = {}, 
			Node* parent = nullptr
		);

		virtual Light* addDirectionalLight(
			const glm::vec3& rotation, 
			const LightProperties& properties = {},
			Node* parent = nullptr
		);

		virtual Light* addSpotLight(
			const glm::vec3& position, 
			const glm::vec3& rotation = {},
			const LightProperties& properties = {}, 
			Node* parent = nullptr
		);

		virtual TextureRenderable* addTextureRenderable(
			Texture& texture,
			const std::string& nodeName,
			const glm::vec2& origin = glm::vec2{ 0.5f },
			const glm::bvec2& flip = glm::bvec2{ false },
			const glm::vec4& color = glm::vec4{ 0.0f },
			const glm::vec3& position = glm::vec3{ 0.0f },
			const glm::vec3& rotation = glm::vec3{ 0.0f },
			const glm::vec3& scale = glm::vec3{ 1.0f }
		);

		virtual Camera* addCamera(
			const std::string& nodeName, 
			const glm::vec2& size, 
			float nearPlane, 
			float farPlane,
			const glm::vec3& position = glm::vec3{ 0.0f },
			const glm::vec3& rotation = glm::vec3{ 0.0f },
			Node* parent = nullptr
		);

		virtual CameraController* addCameraController(
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
		bool hasComponent() const
		{
			return hasComponent(typeid(T));
		}

	protected:

		Node* mRoot{ nullptr };
		std::unique_ptr<ComponentFactory> mComponentFactory{ nullptr };
		std::vector<std::unique_ptr<Node>> mNodes;
		std::unordered_map<std::type_index, std::vector<std::unique_ptr<Component>>> mComponents;
	};
}