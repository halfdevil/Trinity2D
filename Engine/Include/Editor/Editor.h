#pragma once

namespace Trinity
{
	class IEditorLayout;
	class ResourceCache;

	class IEditor
	{
	public:

		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) = 0;
	};
}