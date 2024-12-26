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
	}

	void Simulation::Draw()
	{	
		if (!m_Nodes)
			return;

		ImGui::BeginChild("Packet Settings Container", ImVec2(250, 0), true);
		ImGui::Text("Source Node");
		ImGui::SameLine(120);
		NodeSelector("##Source Node", &m_Settings.SourceNode, m_Settings.bRandomSourceNode, 90);

		ImGui::Text("Destination Node");
		ImGui::SameLine(120);
		NodeSelector("##Destination Node", &m_Settings.DestinationNode, m_Settings.bRandomDestinationNode, 90);


		ImGui::Text("Protocol");
		ImGui::SameLine(120);
		static const char* protocols[] = { "UDP", "TCP" };

		int32 itemSelectedIdx = +(m_Settings.Protocol);
		const char* comboTypePreviewValue;

		if (m_Settings.bRandomProtocol)	  
			comboTypePreviewValue = "Random";
		else
			comboTypePreviewValue = protocols[itemSelectedIdx];

		ImGui::SetNextItemWidth(90);
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
		ImGui::SetNextItemWidth(90);
		if (m_Settings.bRandomMTU)
		{

			//ImGui::InputText("##MTUSize", &m_Settings.MTU, 0, 0, )
		}

		ImGui::InputInt("##MTUSize", &m_Settings.MTU, 10, 100);
		ImGui::SameLine();
		ImGui::Checkbox("##RandomMTU", &m_Settings.bRandomMTU);
		


		ImGui::EndChild();

	}
							  
	void Simulation::NodeSelector(const char* label, int32* location, bool &random, int32 width)
	{
		std::string comboPreviewValueStr;
		if (random)
			comboPreviewValueStr = "Random";
		else if (*location == -1)
			comboPreviewValueStr = "Select ID";
		else
			comboPreviewValueStr=  std::to_string(*location);

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
