#include "Simulation.h"

#include <imgui.h>

namespace VNR
{

	Simulation::Simulation()
		: m_Nodes(nullptr), m_Channels(nullptr)
	{

	}

	void Simulation::ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		m_Nodes = &nodes;
		m_Channels = &channels;

		PacketTransmissionResult res = {
			ENetworkProtocol::UDP,
			0,
			15,
			124000,
			124000 / 1460,
			40 * 124000 / 1460,
			101500,
			670,
			56,
			12
		};
		m_Result.push_back(res);
		m_Result.push_back(res);
		m_Result.push_back(res);
		m_Result.push_back(res);
		m_Result.push_back(res);

	}

	void Simulation::Draw()
	{
		if (!m_Nodes)
			return;

		ControlPanel();
		ImGui::SameLine();
		SimulationTable();

	}

	void Simulation::ControlPanel()
	{
		ImGui::BeginChild("Packet Settings Container", ImVec2(260, 0), true);
		ImGui::Text("Source Node");
		ImGui::SameLine(120);
		NodeSelector("##Source Node", &m_Settings.SourceNode, m_Settings.bRandomSourceNode, 100);

		ImGui::Text("Destination Node");
		ImGui::SameLine(120);
		NodeSelector("##Destination Node", &m_Settings.DestinationNode, m_Settings.bRandomDestinationNode, 100);


		ImGui::Text("Protocol");
		ImGui::SameLine(120);
		static const char* protocols[] = { "UDP", "TCP" };

		int32 itemSelectedIdx = +(m_Settings.Protocol);
		const char* comboTypePreviewValue;

		if (m_Settings.bRandomProtocol)
			comboTypePreviewValue = "Random";
		else
			comboTypePreviewValue = protocols[itemSelectedIdx];

		ImGui::SetNextItemWidth(100);
		if (ImGui::BeginCombo("##Protocol", comboTypePreviewValue))
		{
			for (int32 n = 0; n < 2; n++)
			{
				bool isSelected = (itemSelectedIdx == n);
				if (ImGui::Selectable(protocols[n], isSelected))
					itemSelectedIdx = n;

				if (isSelected)
					ImGui::SetItemDefaultFocus();
			}
			m_Settings.Protocol = ToENetworkProtocol(itemSelectedIdx);
			ImGui::EndCombo();
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomProtocol", &m_Settings.bRandomProtocol);

		ImGui::Text("MTU");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMTU)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MTUSize", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			ImGui::InputInt("##MTUSize", &m_Settings.MTU, 10, 100);
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomMTU", &m_Settings.bRandomMTU);

		ImGui::Text("Message Count");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMessageCount)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MessageCount", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			ImGui::InputInt("##MessageCount", &m_Settings.MessageCount, 1, 10);
		}

		ImGui::SameLine();
		ImGui::Checkbox("##RandomMessageCount", &m_Settings.bRandomMessageCount);


		ImGui::Text("Message Count");
		ImGui::SameLine(120);
		ImGui::SetNextItemWidth(100);
		if (m_Settings.bRandomMessageSize)
		{
			char buff[7] = "Random";
			ImGui::InputText("##MessageSize", buff, 7, ImGuiInputTextFlags_ReadOnly);
		}
		else
		{
			ImGui::InputInt("##MessageSize", &m_Settings.MessageSize, 1, 10);
		}
		ImGui::SameLine();
		ImGui::Checkbox("##RandomMessageSize", &m_Settings.bRandomMessageSize);

		ImGui::Spacing();
		float width = ImGui::GetContentRegionAvail().x;
		if (ImGui::Button("Send", ImVec2{ width, 30.f }))
		{

		}
		ImGui::EndChild();
	}

	void Simulation::SimulationTable()
	{
		ImGui::BeginChild("Simulation Table Container", ImVec2(0, 0), true);

		const ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

		if (ImGui::BeginTable("Simulation Result Table", 10, tableFlags))
		{
			ImGui::TableSetupScrollFreeze(0, 1);

			ImGui::TableSetupColumn("Protocol", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 50);
			ImGui::TableSetupColumn("Source");//, ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("Destination");//, ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("Size");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Packets");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Service");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Payload");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Time");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Retransmission");//, ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableSetupColumn("Loss");//, ImGuiTableColumnFlags_WidthFixed, 20);

			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			ImGui::TableHeadersRow();
			ImGui::PopFont();

			for (int32 i = 0; i < m_Result.size(); i++)
			{
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text();
			}


			ImGui::EndTable();
		}

		ImGui::EndChild();
	}

	void Simulation::NodeSelector(const char* label, int32* location, bool& random, int32 width)
	{
		std::string comboPreviewValueStr;
		if (random)
			comboPreviewValueStr = "Random";
		else if (*location == -1)
			comboPreviewValueStr = "Select ID";
		else
			comboPreviewValueStr = std::to_string(*location);

		const char* comboPreviewValue = comboPreviewValueStr.c_str();

		ImGui::SetNextItemWidth(width);

		if (ImGui::BeginCombo(label, comboPreviewValue))
		{
			for (int i = 0; i < m_Nodes->size(); i++)
			{
				int nodeID = static_cast<int32>((*m_Nodes)[i]->ID);
				bool isSelected = (*location == nodeID);
				if (ImGui::Selectable(std::to_string(nodeID).c_str(), isSelected))
				{
					*location = nodeID;
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();
		ImGui::Checkbox(("##" + String(label) + "Random").c_str(), &random);
	}
}
