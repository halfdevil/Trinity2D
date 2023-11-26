#include "Widgets/Inspector.h"
#include "Scene/Scene.h"
#include "Scene/Node.h"
#include "Scene/ComponentFactory.h"
#include "Editor/Editor.h"
#include "Core/EditorLayout.h"
#include "Core/EditorTheme.h"
#include "misc/cpp/imgui_stdlib.h"
#include "IconsFontAwesome6.h"

namespace Trinity
{
	Inspector::Inspector()
	{
		mEditor = std::make_unique<NodeEditor>();
	}

	void Inspector::setLayout(EditorLayout& layout)
	{
		mLayout = &layout;
	}

	void Inspector::setResourceCache(ResourceCache& cache)
	{
		mResourceCache = &cache;
	}

	void Inspector::setScene(Scene& scene)
	{
		mScene = &scene;
		mEditor->setScene(scene);
		mComponentNames = mScene->getComponentFactory()->getComponentNames();

		setSelectedNode(mScene->getRoot());
	}

	void Inspector::setSelectedNode(Node* node)
	{
		mSelectedNode = node;
		if (mSelectedNode != nullptr)
		{
			mSelectedNodeName = mSelectedNode->getName();
		}
	}

	void Inspector::draw()
	{
		if (!isEnabled())
		{
			return;
		}

		ImGui::Begin(mTitle.c_str(), &mEnabled);
		{
			if (mSelectedNode != nullptr)
			{
				ImGui::TextUnformatted("Name: ");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				
				if (ImGui::InputText("##name", &mSelectedNodeName))
				{
					mSelectedNode->setName(mSelectedNodeName);
				}

				if (ImGui::BeginCombo("##components-combo", mSelectedComponentName.c_str()))
				{
					for (auto& componentName : mComponentNames)
					{
						bool isSelected = componentName == mSelectedComponentName;

						if (ImGui::Selectable(componentName.c_str(), isSelected))
						{
							mSelectedComponentName = componentName;
						}

						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				ImGui::SameLine();

				if (ImGui::Button("Add Component"))
				{
					addComponent();
				}

				ImGui::Separator();

				mEditor->setNode(*mSelectedNode);
				mEditor->onInspectorGui(*mLayout, *mResourceCache);
			}

			ImGui::End();
		}
	}

	void Inspector::addComponent()
	{
		if (mSelectedNode != nullptr && !mSelectedComponentName.empty())
		{
			auto* componentFactory = mScene->getComponentFactory();
			auto component = componentFactory->createComponentByName(mSelectedComponentName);

			if (component != nullptr)
			{
				mScene->addComponent(std::move(component), *mSelectedNode);
			}
		}
	}
}