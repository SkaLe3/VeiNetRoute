#pragma once
#include "VNR-Core/Core/Layer.h"
#include "VNR-App/VNR/GraphEditor/GraphEditor.h"
#include "VNR-App/VNR/GraphEditor/Properties.h"
#include "VNR-App/VNR/GraphEditor/NetworkManager.h"
#include "VNR-App/VNR/GraphEditor/Topology.h"
#include "VNR-App/VNR/GraphEditor/Simulation.h"
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
		void CreateChannel(ChannelData data);
		void CreateNode(NetworkNodeData data);
		void DestroyChannel(Channel* channel);
		void DestroyNode(NetworkNode* node);

	private:
		void GraphEditorWindow();
		void SimualtionWindow();
		void NetworkManagerWindow();
		void TopologyWindow();
		void PropertiesWindow();

		ImGuiWindowFlags m_GraphEditorWindowFlags;


		UniquePtr<Network> m_Network;
		GraphEditor m_Editor;
		TopologySettigs m_Topology;
		Properties m_Properties;
		NetworkManager m_NetworkManager;
		Simulation m_Simulation;

	};
}