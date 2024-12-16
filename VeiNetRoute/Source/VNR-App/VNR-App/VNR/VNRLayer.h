#pragma once
#include "VNR-Core/Core/Layer.h"
#include "VNR-App/VNR/GraphEditor/GraphEditor.h"
#include "VNR-App/VNR/Network/Topology.h"
#include "VNR-App/VNR/Network/Network.h"

#include <imgui.h>

#include <vector>

 struct ImDrawList;

namespace VNR
{

	class VNRLayer : public Layer
	{
	public:
		VNRLayer();
		void OnAttach() override;
		void OnGUI() override;


	public:
		void GenerateNetwork(TopologyData data);

	private:
		void GraphEditorWindow();
		void SimualtionWindow();
		void ConnectionManagerWindow();
		void TopologyWindow();

		ImGuiWindowFlags m_GraphEditorWindowFlags;
		float canvasZoom = 1.f;


		UniquePtr<Network> m_Network;
		GraphEditor m_Editor;
		TopologySettigs m_Topology;

	};
}