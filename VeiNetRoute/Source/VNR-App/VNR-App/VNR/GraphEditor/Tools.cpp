#include "Tools.h"
#include "VNR-App/VNR/VNRLayer.h"


#include <imgui.h>	 

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <unordered_map>

namespace VNR
{

	Tools::Tools(VNRLayer* layer)
		:m_AppLayer(layer)
	{
		m_RoutingDataPath = "Content/VNR/RoutingTables.csv";
		m_Nodes = nullptr;
		m_Channels = nullptr;
	}

	void Tools::ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		m_Nodes = &nodes;
		m_Channels = &channels;
	}
	void Tools::Draw()
	{
		static float fakeTimer = 10;
		ImGui::Text("Synchronization Update Timer");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(100);
		if (m_AppLayer->GetNetworkRef())
			ImGui::SliderFloat("##Synchronization Update Timer", &m_AppLayer->GetNetworkRef()->SynchronizationTimer, 1.f, 30.f, "%.1f");
		else
			ImGui::SliderFloat("##Synchronization Update Timer", &fakeTimer, 1.f, 30.f, "%.1f");

		ImGui::SameLine();

		if (ImGui::Button("Export Routing Table"))
		{
			ExportRoutingTablesToCSV(m_RoutingDataPath);
		}

		ImGui::SameLine(0, 20);
		if (!m_Nodes)
			return;
		NodeSelector("Source", &m_SourceNodeID, 80);
		ImGui::SameLine();
		NodeSelector("Destination", &m_DestinationNodeID, 80);
		ImGui::SameLine();
		if (ImGui::Button("Find Shortest"))
		{
			ClearHighlight();
			int32 nextID = m_SourceNodeID;
			while (nextID != m_DestinationNodeID)
			{
				NetworkNode* node = FindNode(nextID);
				if (!node)
					break;
				int32 nextHop = node->RoutingTable[m_DestinationNodeID].NextHopID;

				Channel* channel = FindChannel(node, nextHop);
				if (!channel)
					break;
				channel->bHighlighted = true;

				if (nextHop < 0)
					break; // Node is unreachable

				nextID = nextHop;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Find Fewest Hops"))
		{
			ClearHighlight();
			NetworkNode* source = FindNode(m_SourceNodeID);
			NetworkNode* destination = FindNode(m_DestinationNodeID);
			std::vector<Channel*> path = FindFewestHopsPath(source, destination);
			for (Channel* c : path)
			{
				c->bHighlighted = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			ClearHighlight();
		}
	}

	void Tools::ExportRoutingTablesToCSV(const std::filesystem::path& filename)
	{
		if (!std::filesystem::exists(filename.parent_path()))
		{
			std::filesystem::create_directories(filename.parent_path());
		}

		std::ofstream file(filename);
		VNR_ASSERT(file.is_open(), "Error writing Routing Tables to file {0}");

		file << "NodeID, DestinationID, NextHopID, Distance\n";

		if (!m_AppLayer->GetNetworkRef())
		{
			file.close();
			return;
		}
		auto routingTables = m_AppLayer->GetNetworkRef()->GetAllRoutingTables();

		for (const auto& [node_id, routing_table] : routingTables)
		{
			for (const auto& entry : routing_table)
			{
				file << node_id << ','
					<< entry.DestinationID << ','
					<< entry.NextHopID << ','
					<< entry.Cost << '\n';
			}
		}

		file.close();
	}

	void Tools::NodeSelector(const char* label, int32* location, int32 width)
	{
		std::string comboPreviewValueStr = (*location == -1)
			? "Select ID"
			: std::to_string(*location);

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
	}

	NetworkNode* Tools::FindNode(int32 nodeID)
	{
		auto it = std::find_if(m_Nodes->begin(), m_Nodes->end(), [&](const UniquePtr<NetworkNode>& node)
							   {
								   return node->ID == nodeID;
							   });

		if (it == m_Nodes->end())
			return nullptr;
		return it->get();
	}

	Channel* Tools::FindChannel(NetworkNode* node1, int32 node2)
	{
		auto it = std::find_if(node1->Channels.begin(), node1->Channels.end(), [&](const Channel* c)
							   {
								   return c->Node1->ID == node1->ID && c->Node2->ID == node2 || c->Node1->ID == node2 && c->Node2->ID == node1->ID;
							   });
		if (it == node1->Channels.end())
			return nullptr;
		return *it;
	}

	void Tools::ClearHighlight()
	{
		for (auto& channel : *m_Channels)
		{
			if (channel->bHighlighted)
				channel->bHighlighted = false;
		}
	}

	std::vector<Channel*> Tools::FindFewestHopsPath(NetworkNode* source, NetworkNode* destination)
	{
		if (!source || !destination)
		{
			return {};
		}

		std::queue<std::pair<NetworkNode*, std::vector<Channel*>>> bfsQueue;
		std::unordered_map<NetworkNode*, bool> visited;

		bfsQueue.push({ source, {} });
		visited[source] = true;

		while (!bfsQueue.empty())
		{
			auto [currentNode, currentPath] = bfsQueue.front();
			bfsQueue.pop();

			if (currentNode == destination)
			{
				return currentPath;
			}

			for (Channel* channel : currentNode->Channels)
			{
				NetworkNode* neighbor = (channel->Node1 == currentNode) ? channel->Node2 : channel->Node1;

				if (!channel->bEnabled || !neighbor->bEnabled || visited[neighbor])
				{
					continue;
				}
				visited[neighbor] = true;

				std::vector<Channel*> newPath = currentPath;
				newPath.push_back(channel);

				bfsQueue.push({ neighbor, newPath });
			}
		}
		return {};
	}

}

