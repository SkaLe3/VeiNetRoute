#include "NetworkManager.h"
#include "VNR-Core/UI/UI.h"

#include <imgui.h>

#include <algorithm>

namespace VNR
{

	NetworkManager::NetworkManager()
	{
		m_Nodes = nullptr;
		m_Channels = nullptr;
	}

	void NetworkManager::ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		m_Nodes = &nodes;
		m_Channels = &channels;
	}

	void NetworkManager::Draw()
	{
		if (ImGui::BeginTabBar("Network Tab Bar"))
		{
			if (ImGui::BeginTabItem("Connections"))
			{
				ConnectionTab();
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Nodes"))
			{
				NodeTab();
				ImGui::EndTabItem();

			}
			ImGui::EndTabBar();
		}
	}

	void NetworkManager::AddChannelPopup()
	{
		if (ImGui::BeginPopup("Add Channel"))
		{
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			UI::TextCentered("Add New Channel");
			ImGui::PopFont();
			ImGui::Dummy(ImVec2(0, 10));



			for (int32 node = 0; node < 2; node++)
			{
				std::string comboPreviewValueStr = (m_NewChannel.NodesID[node] == -1)
					? "Select Node ID"
					: std::to_string(m_NewChannel.NodesID[node]);
				const char* comboPreviewValue = comboPreviewValueStr.c_str();
				ImGui::SetNextItemWidth(120);
				if (ImGui::BeginCombo(("Node " + std::to_string(node + 1)).c_str(), comboPreviewValue))
				{
					for (int i = 0; i < m_Nodes->size(); i++)
					{
						int nodeID = static_cast<int32>((*m_Nodes)[i]->ID);
						bool isSelected = (m_NewChannel.NodesID[node] == nodeID);
						if (ImGui::Selectable(std::to_string(nodeID).c_str(), isSelected))
						{
							m_NewChannel.NodesID[node] = nodeID;
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}
			}
			static const char* types[] = { "Duplex", "Half Duplex" };

			int itemSelectedIdx = +(m_NewChannel.Type);
			const char* comboTypePreviewValue = types[itemSelectedIdx];
			ImGui::SetNextItemWidth(120);
			if (ImGui::BeginCombo("Type", comboTypePreviewValue))
			{
				for (int32 n = 0; n < 2; n++)
				{
					bool isSelected = (itemSelectedIdx == n);
					if (ImGui::Selectable(types[n], isSelected))
						itemSelectedIdx = n;

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				m_NewChannel.Type = ToEChannelType(itemSelectedIdx);
				ImGui::EndCombo();
			}
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt("Weight", &m_NewChannel.Weight, 1, 1))
			{
				m_NewChannel.Weight = std::clamp(m_NewChannel.Weight, 0, 100);
			}
			ImGui::SetNextItemWidth(120);
			if (ImGui::InputFloat("Error Probability", &m_NewChannel.ErrorProbability, 0.1, 0.1, "%.2f"))
			{
				m_NewChannel.ErrorProbability = std::clamp(m_NewChannel.ErrorProbability, 0.f, 1.f);
			}

			ImGui::Spacing();

			ImVec2 submitSize = ImGui::GetContentRegionAvail();
			submitSize.y = 30;
			if (ImGui::Button("Submit", submitSize))
			{
				if (m_NewChannel.NodesID[0] >= 0 && m_NewChannel.NodesID[1] >= 0 &&
					m_NewChannel.NodesID[0] != m_NewChannel.NodesID[1])
				{
					OnCreateChannel.Broadcast(m_NewChannel);
					m_NewChannel = ChannelData();
					ImGui::CloseCurrentPopup();
				}

			}
			ImGui::EndPopup();
		}
	}

	void NetworkManager::AddNodePopup()
	{
		if (ImGui::BeginPopup("Add Node"))
		{
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			UI::TextCentered("Add New Node");
			ImGui::PopFont();
			ImGui::Dummy(ImVec2(0, 10));

			ImGui::SetNextItemWidth(120);
			if (ImGui::InputInt("Node ID", &m_NewNode.ID, 1, 1))
			{
				m_NewNode.ID = std::clamp(m_NewNode.ID, 0, 100);
			}
			ImGui::Checkbox("Enable", &m_NewNode.bEnabled);

			ImGui::Spacing();

			ImVec2 submitSize = ImGui::GetContentRegionAvail();
			submitSize.y = 30;
			if (ImGui::Button("Submit", submitSize))
			{	
				if (!Contains(m_NewNode.ID) && m_NewNode.ID >= 0)
				{
					OnCreateNode.Broadcast(m_NewNode);
					m_NewNode = NetworkNodeData();
					ImGui::CloseCurrentPopup();
				}
			}
			ImGui::EndPopup();
		}

	}

	void NetworkManager::ConnectionTab()
	{
		ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 30);
		ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
		ImGui::Spacing();
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Add Channel", buttonSize))
		{
			if (m_Nodes && m_Nodes->size() >= 2)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				ImVec2 popupPos;
				popupPos.x = viewport->Pos.x + viewport->Size.x / 2.0f;
				popupPos.y = viewport->Pos.y + viewport->Size.y / 2.0f;
				ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				ImGui::OpenPopup("Add Channel");
			}
		}
		ImGui::PopFont();
		ImGui::Spacing();
		AddChannelPopup();

		int32 removeIndex = -1;
		ImGui::BeginChild("Connection Container", ImVec2(0, 0), true);
		const ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("Connection Table", 7, tableFlags))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			ImGui::TableSetupColumn(" N", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort, 20);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize | ImGuiTableColumnFlags_NoSort, 60);
			ImGui::TableSetupColumn("Node 1", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60);
			ImGui::TableSetupColumn("Node 2", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60);
			ImGui::TableSetupColumn("Type");
			ImGui::TableSetupColumn("Weight");
			ImGui::TableSetupColumn("Error", ImGuiTableColumnFlags_WidthFixed, 40);
			ImGui::TableHeadersRow();
			ImGui::PopFont();

			if (!m_Channels)
			{
				ImGui::EndTable();
				ImGui::EndChild();
				return;
			}

			ImGuiTableSortSpecs* sortSpecs = ImGui::TableGetSortSpecs();
			if (sortSpecs->SpecsDirty)
			{
				// Update sorting order
				sortSpecs->SpecsDirty = false;

				// Determine the column to sort by and the sorting order
				int sortColumn = sortSpecs->Specs[0].ColumnIndex;
				bool ascending = (sortSpecs->Specs[0].SortDirection == ImGuiSortDirection_Ascending);

				if (sortColumn == 2)  // Sort by "Node 1"
				{
					if (ascending)
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Node1->ID < b->Node1->ID;
								  });
					else
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Node1->ID > b->Node1->ID;
								  });
				}
				else if (sortColumn == 3)  // Sort by "Node 2"
				{
					if (ascending)
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Node2->ID < b->Node2->ID;
								  });
					else
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Node2->ID > b->Node2->ID;
								  });
				}
				else if (sortColumn == 5)  // Sort by "Weight"
				{
					if (ascending)
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Weight < b->Weight;
								  });
					else
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->Weight > b->Weight;
								  });
				}
				else if (sortColumn == 6)  // Sort by "Error"
				{
					if (ascending)
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->ErrorProbability < b->ErrorProbability;
								  });
					else
						std::sort(m_Channels->begin(), m_Channels->end(), [](const UniquePtr<Channel>& a, const UniquePtr<Channel>& b)
								  {
									  return a->ErrorProbability > b->ErrorProbability;
								  });
				}
			}

			static const char* types[] = { "Duplex", "Half Duplex" };
			for (int32 i = 0; i < m_Channels->size(); i++)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(1);
				ImVec2 removeSize = ImGui::GetContentRegionAvail();
				removeSize.y = 0;
				UI::ShiftCursorX(2);
				if (ImGui::Button(("Remove##" + std::to_string(i)).c_str(), removeSize))
				{
					removeIndex = i;
				}

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%d", i+1);

				UniquePtr<Channel>& channel = (*m_Channels)[i];

				ImGui::TableSetColumnIndex(2);
				ImGui::Text(" %d", channel->Node1->ID);

				ImGui::TableSetColumnIndex(3);
				ImGui::Text("%d", channel->Node2->ID);

				ImGui::TableSetColumnIndex(4);
				int itemSelectedIdx = +(channel->Type);
				const char* comboPreviewValue = types[itemSelectedIdx];
				ImGui::SetNextItemWidth(-1);
				if (ImGui::BeginCombo(("##Type" + std::to_string(channel->Node1->ID * 100) + std::to_string(channel->Node2->ID)).c_str(), comboPreviewValue, ImGuiComboFlags_PopupAlignLeft))
				{
					for (int32 n = 0; n < 2; n++)
					{
						bool isSelected = (itemSelectedIdx == n);
						if (ImGui::Selectable(types[n], isSelected))
							itemSelectedIdx = n;

						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					channel->Type = ToEChannelType(itemSelectedIdx);
					ImGui::EndCombo();
				}
				ImGui::SetNextItemWidth(-1);
				ImGui::TableSetColumnIndex(5);
				if (ImGui::InputInt(("##Weight" + std::to_string(channel->Node1->ID * 100) + std::to_string(channel->Node2->ID)).c_str(), &channel->Weight, 1, 1))
				{
					channel->Weight = std::clamp(channel->Weight, 0, 100);
				}
				ImGui::SetNextItemWidth(-1);
				ImGui::TableSetColumnIndex(6);
				if (ImGui::InputFloat(("##Error" + std::to_string(channel->Node1->ID * 100) + std::to_string(channel->Node2->ID)).c_str(), &channel->ErrorProbability, 0, 0, "%.2f"))
				{
					channel->ErrorProbability = std::clamp(channel->ErrorProbability, 0.f, 1.f);
				}
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();

		if (removeIndex >= 0)
		{
			OnDestroyChannel.Broadcast((*m_Channels)[removeIndex].get());
		}
	}

	void NetworkManager::NodeTab()
	{
		ImVec2 buttonSize = ImVec2(ImGui::GetContentRegionAvail().x, 30);
		ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
		ImGui::PushFont(boldFont);
		ImGui::Spacing();
		if (ImGui::Button("Add Node", buttonSize))
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 popupPos;
			popupPos.x = viewport->Pos.x + viewport->Size.x / 2.0f;
			popupPos.y = viewport->Pos.y + viewport->Size.y / 2.0f;
			ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			ImGui::OpenPopup("Add Node");
		}
		ImGui::Spacing();
		ImGui::PopFont();
		AddNodePopup();


		int32 removeIndex = -1;
		ImGui::BeginChild("Node Container", ImVec2(0, 0), true);
		const ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable |
			ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_SizingFixedFit |
			ImGuiTableFlags_RowBg | ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY;
		if (ImGui::BeginTable("Node Table", 3, tableFlags))
		{
			ImGui::TableSetupScrollFreeze(0, 1);
			ImFont* boldFont = ImGui::GetIO().Fonts->Fonts[0];
			ImGui::PushFont(boldFont);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60);
			ImGui::TableSetupColumn("ID", ImGuiTableColumnFlags_WidthFixed, 100);
			ImGui::TableSetupColumn("Status");
			ImGui::TableHeadersRow();
			ImGui::PopFont();

			if (!m_Nodes)
			{
				ImGui::EndTable();
				ImGui::EndChild();
				return;
			}
			for (int32 i = 0; i < m_Nodes->size(); i++)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImVec2 removeSize = ImGui::GetContentRegionAvail();
				removeSize.y = 0;
				UI::ShiftCursorX(2);
				if (ImGui::Button(("Remove##" + std::to_string(i)).c_str(), removeSize))
				{
					removeIndex = i;
				}

				UniquePtr<NetworkNode>& node = (*m_Nodes)[i];

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(" %d", node->ID);

				ImGui::TableSetColumnIndex(2);
				ImGui::Checkbox(("##Remove" + std::to_string(i)).c_str(), &node->bEnabled);
			}
			ImGui::EndTable();
		}
		ImGui::EndChild();
		if (removeIndex >= 0)
		{
			OnDestroyNode.Broadcast((*m_Nodes)[removeIndex].get());
		}
	}

	bool NetworkManager::Contains(int32 ID)
	{
		auto it = std::find_if(m_Nodes->begin(), m_Nodes->end(),
		[&](const auto& ptr)
		{
			return ID == ptr->ID; 
		});
		return it != m_Nodes->end();
	}

}

