#include "VNRLayer.h"

#include <imgui_internal.h>

#include <vector>
#include <algorithm>


namespace VNR
{


	VNRLayer::VNRLayer() : Layer("VNRLayer"),
		m_GraphEditorWindowFlags(ImGuiWindowFlags_NoMove), m_Tools(this)
	{
		std::vector<int32> weights = { 3, 5, 6, 7, 8, 10, 11, 15, 18, 21 };
		std::vector<int32> rNetworks = { 9, 9, 9 };
		m_Topology.InitWeights(weights);
		m_Topology.InitRegionalNetworks(rNetworks);
		m_Topology.OnGenerate.Add([this](TopologyData data) { GenerateNetwork(data); });
		m_NetworkManager.OnCreateChannel.Add([this](ChannelData data) { CreateChannel(data); });
		m_NetworkManager.OnCreateNode.Add([this](NetworkNodeData data) { CreateNode(data); });
		m_NetworkManager.OnDestroyChannel.Add([this](Channel* channel) { DestroyChannel(channel); });
		m_NetworkManager.OnDestroyNode.Add([this](NetworkNode* node) { DestroyNode(node); });
		m_Properties.OnDeleteNode.Add([this](NetworkNode* node) { DestroyNode(node); });

	}

	void VNRLayer::OnAttach()
	{

	}

	void VNRLayer::OnGUI()
	{
		GraphEditorWindow();
		SimualtionWindow();
		NetworkManagerWindow();
		TopologyWindow();
		PropertiesWindow();
		ToolsWindow();
	}


	void VNRLayer::OnUpdate(float deltaTime)
	{
		if (m_Network)
			m_Network->SimulationStep(deltaTime);
	}

	void VNRLayer::GenerateNetwork(TopologyData data)
	{
		m_Network = MakeUnique<Network>(data);
		m_NetworkManager.ProvideNetworkData(m_Network->Nodes, m_Network->Channels);
		m_Tools.ProvideNetworkData(m_Network->Nodes, m_Network->Channels);
		m_Simulation.ProvideNetworkData(m_Network->Nodes, m_Network->Channels);
		m_Editor.GenerateGraph(m_Network->Nodes, m_Network->Channels, m_Network->GetTopology());
	}

	void VNRLayer::CreateChannel(ChannelData data)
	{
		Channel* channel = m_Network->AddChannel(data);
		if (!channel)
			return;
		m_Editor.AddEdge(channel);
	}

	void VNRLayer::CreateNode(NetworkNodeData data)
	{
		NetworkNode* node = m_Network->AddNode(data);
		if (!node)
			return;
		m_Editor.AddNode(node);
	}

	void VNRLayer::DestroyChannel(Channel* channel)
	{
		m_Editor.RemoveEdge(channel->Visuals);
		m_Network->RemoveChannel(channel);
	}

	void VNRLayer::DestroyNode(NetworkNode* node)
	{
		for (int i = 0; i < node->Channels.size(); )
		{
			DestroyChannel(node->Channels[0]);
		}
		m_Editor.RemoveNode(node->Visuals);
		m_Network->RemoveNode(node);
	}

	UniquePtr<Network>& VNRLayer::GetNetworkRef()
	{
		 return m_Network;
	}

	void VNRLayer::GraphEditorWindow()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 originalPadding = style.WindowPadding;  // Save the original padding

		style.WindowPadding = ImVec2(0.0f, 0.0f);

		ImVec4 customColor(0.105f, 0.1f, 0.11f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, customColor);
		ImGui::Begin("Graph Editor", nullptr, m_GraphEditorWindowFlags);



		ImVec2 titleBarMin = ImGui::GetWindowPos();
		ImVec2 titleBarMax = ImVec2(titleBarMin.x + ImGui::GetWindowWidth(), titleBarMin.y + ImGui::GetFrameHeight());
		if (ImGui::IsMouseHoveringRect(titleBarMin, titleBarMax, false))
			m_GraphEditorWindowFlags = ImGuiWindowFlags_None;
		else
			m_GraphEditorWindowFlags |= ImGuiWindowFlags_NoMove;

		m_Editor.Draw();

		ImGui::End();
		ImGui::PopStyleColor();
		style.WindowPadding = originalPadding;
	}

	void VNRLayer::SimualtionWindow()
	{
		ImGui::Begin("Simulation");

		m_Simulation.Draw();
		ImGui::End();
	}

	void VNRLayer::NetworkManagerWindow()
	{
		ImGui::Begin("Network Manager");
		m_NetworkManager.Draw();
		ImGui::End();
	}

	void VNRLayer::TopologyWindow()
	{
		ImGui::Begin("Topology");
		m_Topology.Draw();
		ImGui::End();
	}

	void VNRLayer::PropertiesWindow()
	{
		auto* selectedNode = m_Editor.GetSelectedNode();
		if (selectedNode)
			m_Properties.SelectNode(selectedNode->Data);
		else
			m_Properties.DeselectNode();

		ImGui::Begin("Properties");
		m_Properties.Draw();
		ImGui::End();
	}

	void VNRLayer::ToolsWindow()
	{
		ImGui::Begin("Tools");
		m_Tools.Draw();
		ImGui::End();
	}

}