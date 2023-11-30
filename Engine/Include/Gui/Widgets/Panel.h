#pragma once

#include "Gui/Widget.h"

namespace Trinity
{
	class Texture;
	class PanelEditor;
	class PanelSerializer;

	class Panel : public Widget
	{
	public:

		friend class PanelEditor;
		friend class PanelSerializer;

		Panel() = default;
		virtual ~Panel() = default;

		Panel(const Panel&) = delete;
		Panel& operator = (const Panel&) = delete;

		Panel(Panel&&) = default;
		Panel& operator = (Panel&&) = default;

		Texture* getBackgroundTexture() const
		{
			return mBackgroundTexture;
		}

		const glm::vec4& getBackgroundColor() const
		{
			return mBackgroundColor;
		}

		virtual IEditor* getEditor(Gui& gui) override;
		virtual ISerializer* getSerializer(Gui& gui) override;

		virtual void setBackgroundTexture(Texture& texture);
		virtual void setBackgroundColor(const glm::vec4& color);

		virtual std::type_index getType() const override;
		virtual UUIDv4::UUID getTypeUUID() const override;

		virtual void preDraw(GuiRenderer& renderer) override;
		virtual void postDraw(GuiRenderer& renderer) override;

	public:

		inline static UUIDv4::UUID UUID = UUIDv4::UUID::fromStrFactory("2f5374fa-7155-4b88-84ef-eac7a051ceca");

	protected:

		Texture* mBackgroundTexture{ nullptr };
		glm::vec4 mBackgroundColor{ 0.0f };
	};

	class PanelEditor : public WidgetEditor
	{
	public:

		PanelEditor() = default;
		virtual ~PanelEditor() = default;

		PanelEditor(const PanelEditor&) = delete;
		PanelEditor& operator = (const PanelEditor&) = delete;

		PanelEditor(PanelEditor&&) = default;
		PanelEditor& operator = (PanelEditor&&) = default;

		virtual void setPanel(Panel& panel);
		virtual void onInspectorGui(const IEditorLayout& layout, ResourceCache& cache) override;

	protected:

		Panel* mPanel{ nullptr };
		std::string mSelectedTextureFile;
	};

	class PanelSerializer : public WidgetSerializer
	{
	public:

		PanelSerializer() = default;
		virtual ~PanelSerializer() = default;

		PanelSerializer(const PanelSerializer&) = delete;
		PanelSerializer& operator = (const PanelSerializer&) = delete;

		PanelSerializer(PanelSerializer&&) = default;
		PanelSerializer& operator = (PanelSerializer&&) = default;

		virtual void setPanel(Panel& panel);
		virtual bool read(FileReader& reader, ResourceCache& cache) override;
		virtual bool write(FileWriter& writer) override;

		virtual bool read(json& object, ResourceCache& cache) override;
		virtual bool write(json& object) override;

	protected:

		Panel* mPanel{ nullptr };
	};
}