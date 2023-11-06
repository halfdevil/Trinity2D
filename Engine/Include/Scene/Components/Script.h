#pragma once

#include "Scene/Component.h"

namespace Trinity
{
	class Node;

	class Script : public Component
	{
	public:

		Script() = default;
		virtual ~Script() = default;

		Script(const Script&) = delete;
		Script& operator = (const Script&) = delete;

		Script(Script&&) = default;
		Script& operator = (Script&&) = default;

		Node* getNode() const
		{
			return mNode;
		}

		virtual std::type_index getType() const override;
		virtual std::string getTypeName() const override;

		virtual void init();
		virtual void update(float deltaTime);
		virtual void resize(uint32_t width, uint32_t height);
		virtual void setNode(Node& node);

	public:

		static std::string getStaticType();

	protected:

		Node* mNode{ nullptr };
	};
}