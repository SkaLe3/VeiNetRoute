#include "Properties.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include <imgui.h>

namespace VNR
{
	/* Helper Functions */
	void PrettyInt(const char* label, int32* value, int32 width)
	{
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.8f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 3.0f));
		ImGui::PushItemWidth(width);
		ImGui::InputInt(label, value, 0, 0, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(2);
	}



	Properties::Properties()
	{
		m_SelectedNode = nullptr;
	}

	void Properties::SelectNode(NetworkNode* node)
	{
		m_SelectedNode = node;
	}

	void Properties::DeselectNode()
	{
		m_SelectedNode = nullptr;
	}

	void Properties::Draw()
	{
		ImGui::SeparatorText("Node");

		ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];

		// Check if no node is selected
		if (!m_SelectedNode)
		{
			ImGui::PushFont(boldFont);
			ImGui::Text("Node is not selected");
			ImGui::PopFont();
			return;
		}
		bool shouldDelete = false;

		// Align text and display node ID
		ImGui::AlignTextToFramePadding();
		ImGui::PushFont(boldFont);
		ImGui::Text("Selected Node:");
		ImGui::SameLine();
		ImGui::PopFont();

		int32 nodeId = m_SelectedNode->ID;
		PrettyInt("##Selected Node", &nodeId, 30);
		ImGui::SameLine();
		ImGui::Text(":");
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			shouldDelete = true;
		}

		ImGui::Checkbox("Enable", &m_SelectedNode->bEnabled);

		ImGui::SeparatorText("Connections");

		// Display headers for connection details
		ImGui::PushFont(boldFont);
		int32 headerOffset = ImGui::GetCursorPosX();
		ImGui::SetCursorPosX(headerOffset + 2);
		ImGui::Text("To Node"); ImGui::SameLine();
		ImGui::SetCursorPosX(headerOffset + 84);
		ImGui::Text("Type"); ImGui::SameLine();
		ImGui::SetCursorPosX(headerOffset + 192);
		ImGui::Text("Weight"); ImGui::SameLine();
		ImGui::SetCursorPosX(headerOffset + 300);
		ImGui::Text("Error Probability");
		ImGui::Spacing();
		ImGui::PopFont();

		const char* items[] = { "Duplex", "Half Duplex" };

		// Loop through channels and display each connection
		for (uint32 i = 0; i < m_SelectedNode->Channels.size(); i++)
		{
			Channel* channel = m_SelectedNode->Channels[i];
			uint32 connectedID = (channel->Node1->ID == m_SelectedNode->ID) ? channel->Node2->ID : channel->Node1->ID;
			int32 connectedIDtext = connectedID;

			// Display "To Node" and connection details
			ImGui::Checkbox(("##Enabled" + std::to_string(connectedID)).c_str(), &channel->bEnabled);
			ImGui::SameLine();
			PrettyInt("##ToNode", &connectedIDtext, 30);
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1);
			ImGui::Text(":");
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 1);

			// Combo box for channel type
			int itemSelectedIdx = +(channel->Type);
			const char* comboPreviewValue = items[itemSelectedIdx];

			ImGui::SetNextItemWidth(100);
			if (ImGui::BeginCombo(("##Node" + std::to_string(connectedID)).c_str(), comboPreviewValue, ImGuiComboFlags_PopupAlignLeft))
			{
				for (int32 n = 0; n < 2; n++)
				{
					bool isSelected = (itemSelectedIdx == n);
					if (ImGui::Selectable(items[n], isSelected))
						itemSelectedIdx = n;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				channel->Type = ToEChannelType(itemSelectedIdx);
				ImGui::EndCombo();
			}

			// Input fields for weight and error probability
			ImGui::SameLine();
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt(("##Weight" + std::to_string(connectedID)).c_str(), &channel->Weight, 1, 1))
			{
				channel->Weight = std::clamp(channel->Weight, 0, 100);
			}
			ImGui::SameLine();
			if (ImGui::InputFloat(("##Error" + std::to_string(connectedID)).c_str(), &channel->ErrorProbability, 0.1f, 0.1f, "%.2f"))
			{
				channel->ErrorProbability = std::clamp(channel->ErrorProbability, 0.f, 1.f);
			}
			ImGui::PopItemWidth();
		}
		if (shouldDelete)
			OnDeleteNode.Broadcast(m_SelectedNode);
	}


}

