#include "VNRLayer.h"

#include <imgui_internal.h>

#include <vector>
#include <algorithm>


namespace VNR
{


	VNRLayer::VNRLayer() : Layer("VNRLayer"),
		m_GraphEditorWindowFlags(ImGuiWindowFlags_NoMove)
	{
		std::vector<int32> weights = { 3, 5, 6, 7, 8, 10, 11, 15, 18, 21 };
		m_Topology.InitWeights(weights);
		m_Topology.InitRegionalNetworks(3);
		m_Topology.InitNodesInNetwork(9);
		m_Topology.OnGenerate.Add([this](TopologyData data) { GenerateNetwork(data); });

	}

	void VNRLayer::OnAttach()
	{

	}

	void VNRLayer::OnGUI()
	{
		GraphEditorWindow();
		SimualtionWindow();
		ConnectionManagerWindow();
		TopologyWindow();
	}


	void VNRLayer::GenerateNetwork(TopologyData data)
	{
		m_Network = MakeUnique<Network>(data);
		m_Editor.GenerateGraph(m_Network->Nodes, m_Network->Channels);
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

		ImGui::Button("Dummy Button");
		ImGui::Text("Dummy Text");
		ImGui::End();
	}

	void VNRLayer::ConnectionManagerWindow()
	{
		ImGui::Begin("Connection Manager");

		if (ImGui::BeginTable("Connection Table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			// Set up column headers
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Col 1");
			ImGui::TableSetupColumn("Col 2");
			ImGui::TableHeadersRow();

			// Add rows dynamically
			for (int row = 0; row < 10; ++row)
			{
				ImGui::TableNextRow();

				// Fill each column in the row
				ImGui::TableSetColumnIndex(0); // Column 0
				ImGui::Text("%d", row);

				ImGui::TableSetColumnIndex(1); // Column 1
				ImGui::Text("Connection %d", row);

				ImGui::TableSetColumnIndex(2); // Column 2
				ImGui::Text("%.1f", 100.0f - row * 5.0f);
			}

			ImGui::EndTable();
		}
		ImGui::End();
	}

	void VNRLayer::TopologyWindow()
	{
		ImGui::Begin("Topology");
		m_Topology.Draw();
		ImGui::End();
	}

}