#pragma once

#include "Scene/Component.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Trinity
{
	class Node;

	class Transform : public Component
	{
	public:

		Transform() = default;
		virtual ~Transform() = default;

		Transform(const Transform&) = delete;
		Transform& operator = (const Transform&) = delete;

		Transform(Transform&&) = default;
		Transform& operator = (Transform&&) = default;

		const glm::vec3& getTranslation() const
		{
			return mTranslation;
		}

		const glm::quat& getRotation() const
		{
			return mRotation;
		}

		const glm::vec3& getScale() const
		{
			return mScale;
		}

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		glm::mat4 getMatrix() const;
		glm::mat4 getWorldMatrix();

		void setMatrix(const glm::mat4& matrix);
		void setTranslation(const glm::vec3& translation);
		void setRotation(const glm::quat& rotation);
		void setScale(const glm::vec3& scale);
		void invalidateWorldMatrix();

	public:

		static std::string getStaticType();

	protected:

		void updateWorldTransform();

	protected:

		glm::vec3 mTranslation{ 0.0f, 0.0f, 0.0f };
		glm::quat mRotation{ 1.0f, 0.0f, 0.0f, 0.0f };
		glm::vec3 mScale{ 1.0f, 1.0f, 1.0f };
		glm::mat4 mWorldMatrix{ 1.0f };

	private:

		bool mUpdateMatrix{ false };
	};
}