#pragma once

#include "Scene/Component.h"
#include <glm/glm.hpp>

namespace Trinity
{
	class Node;

	class Camera : public Component
	{
	public:

		Camera() = default;
		virtual ~Camera() = default;

		Camera(const Camera&) = delete;
		Camera& operator = (const Camera&) = delete;

		Camera(Camera&&) = default;
		Camera& operator = (Camera&&) = default;

		Node* getNode() const
		{
			return mNode;
		}

		const glm::mat4& getProjection() const
		{
			return mProjection;
		}

		const glm::mat4& getView() const
		{
			return mView;
		}

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual void setProjection(const glm::mat4& projection);
		virtual void setView(const glm::mat4& view);
		virtual void setNode(Node& node);

	public:

		static std::string getStaticType();

	protected:

		Node* mNode{ nullptr };
		glm::mat4 mProjection;
		glm::mat4 mView;
	};
}