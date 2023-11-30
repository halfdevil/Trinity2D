#pragma once

#include "Core/EditorWidget.h"
#include <string>
#include <memory>
#include <vector>

namespace Trinity
{
	class Scene;
	class Node;
	class NodeEditor;
	class ResourceCache;
	class EditorLayout;

	class Inspector : public EditorWidget
	{
	public:

		Inspector() = default;
		virtual ~Inspector() = default;

		Inspector(const Inspector&) = delete;
		Inspector& operator = (const Inspector&) = delete;

		Inspector(Inspector&&) = default;
		Inspector& operator = (Inspector&&) = default;

		virtual void setLayout(EditorLayout& layout);
		virtual void setResourceCache(ResourceCache& cache);
		virtual void draw() override;

	protected:

		virtual void drawInspector();

	protected:

		EditorLayout* mLayout{ nullptr };
		ResourceCache* mResourceCache{ nullptr };
		bool mIsOpen{ true };
	};
}