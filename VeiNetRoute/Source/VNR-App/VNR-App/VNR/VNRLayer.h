#pragma once
#include "VNR-Core/Core/Layer.h"
#include <imgui.h>
#include "VNR-App/VNR/GraphEditor/GraphEditor.h"

 struct ImDrawList;

namespace VNR
{

	class VNRLayer : public Layer
	{
	public:
		VNRLayer();
		void OnAttach() override;
		void OnGUI() override;


	private:
		void GraphEditorWindow();
		void SimualtionWindow();
		void ConnectionManagerWindow();

		ImGuiWindowFlags m_GraphEditorWindowFlags;
		float canvasZoom = 1.f;

		GraphEditor m_Editor;

	};
}