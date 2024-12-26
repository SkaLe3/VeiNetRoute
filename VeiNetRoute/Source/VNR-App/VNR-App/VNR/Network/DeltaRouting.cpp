#include "DeltaRouting.h"
#include "VNR-App/VNR/Network/NetworkNode.h"

#include <limits>
#include <algorithm>


namespace VNR::DeltaRouting
{


	void SynchronizeRoutingTables(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		ValidateTopologyChanges(nodes, channels);

		for (auto& node : nodes)
		{
			if (!node->bEnabled)
				continue;
			NRoutingTable& table = node->RoutingTable;
			for (auto& channel : node->Channels)
			{
				if (!channel->bEnabled)
					continue;
				if (!channel->Node1->bEnabled || !channel->Node2->bEnabled)
					continue;

				NetworkNode* neighbor = channel->Node1 == node.get() ? channel->Node2 : channel->Node1;
				table[neighbor->ID] = { neighbor->ID, neighbor->ID, channel->Weight };
			}
		}

		size_t enabledNodesSize = 0;
		for (auto& node : nodes)
		{
			if (node->bEnabled) enabledNodesSize++;
		}
		for (auto& node : nodes)
		{
			if (!node->bEnabled)
				continue;

			NRoutingTable& sourceTable = node->RoutingTable;
			for (size_t i = 0; i < enabledNodesSize - 1; i++)
			{
				for (auto& channel : channels)
				{
					if (!channel->bEnabled)
						continue;
					if (!channel->Node1->bEnabled || !channel->Node2->bEnabled)
						continue;

					NetworkNode* node1 = channel->Node1;
					NetworkNode* node2 = channel->Node2;

					int32 myCost = sourceTable[node2->ID].Cost;
					int32 newCost = std::max(sourceTable[node1->ID].Cost + channel->Weight, sourceTable[node1->ID].Cost);

					if (newCost < myCost)
					{
						sourceTable[node2->ID].Cost = newCost;
						sourceTable[node2->ID].NextHopID = sourceTable[node1->ID].NextHopID;
					}

					myCost = sourceTable[node1->ID].Cost;
					newCost = std::max(sourceTable[node2->ID].Cost + channel->Weight, sourceTable[node2->ID].Cost);

					if (newCost < myCost)
					{
						sourceTable[node1->ID].Cost = newCost;
						sourceTable[node1->ID].NextHopID = sourceTable[node2->ID].NextHopID;
					}
				}
			}
		}
	}

	void PropagateDelta(std::vector<UniquePtr<NetworkNode>>& nodes, const Delta& delta)
	{
		switch (delta.Type)
		{
		case EDeltaType::ChannelReduction:
			InvalidateChangedRoutes(nodes, delta.AffectedNodes);
			break;
		case EDeltaType::NodeReduction:
			InvalidateDestinationNode(nodes, delta.AffectedNodes);
			break;
		case EDeltaType::ChannelAddition:
			InitializeRoutes(nodes, delta.AffectedNodes);
			break;
		case EDeltaType::NodeAddition:
			InitializeNodes(nodes, delta.AffectedNodes);
			break;

		}
	}

	void InvalidateChangedRoutes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32>& ids)
	{
		for (auto& node : nodes)
		{
			for (auto& [id, route] : node->RoutingTable)
			{
				if (id == node->ID)
					continue;

				bool containsAffected = std::any_of(ids.begin(), ids.end(), [&](int32 n) { return route.DestinationID == n || route.NextHopID == n; });

				if (containsAffected)
				{
					route.NextHopID = -1;
					route.Cost = std::numeric_limits<int32>::max();
				}
			}
		}
	}

	void InvalidateDestinationNode(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32>& ids)
	{
		for (auto& node : nodes)
		{
			for (int32 i = 0; i < ids.size(); i++)
			{
				node->RoutingTable.erase(ids[i]);
			}
			for (auto& [id, route] : node->RoutingTable)
			{
				bool containsAffected = std::any_of(ids.begin(), ids.end(), [&](int32 n) { return route.NextHopID == n; });

				if (containsAffected)
				{
					route.NextHopID = -1;
					route.Cost = std::numeric_limits<int32>::max();
				}
			}
		}
	}

	void InitializeRoutes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32> ids)
	{

	}

	void InitializeNodes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32> ids)
	{
		std::vector<NetworkNode*> nodePtrs;

		for (auto& node : nodes)
		{
			for (int32 id : ids)
			{
				node->RoutingTable[id] = { id, -1, std::numeric_limits<int32>::max() };
				if (node->ID == id)
				{
					nodePtrs.push_back(node.get());
				}
			}

		}

		for (auto& node : nodePtrs)
		{
			for (auto& destinationNode : nodes)
			{
				node->RoutingTable[destinationNode->ID] = { destinationNode->ID, -1,  std::numeric_limits<int32>::max() };
			}
			node->RoutingTable[node->ID] = { node->ID, node->ID, 0 };

		}
	}

	void ValidateTopologyChanges(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		Delta channelDeltaReduction;
		Delta channelDeltaAddition;
		Delta nodeDeltaReduction;
		Delta nodeDeltaAddition;
		channelDeltaReduction.Type = EDeltaType::ChannelReduction;
		channelDeltaAddition.Type = EDeltaType::ChannelAddition;
		nodeDeltaReduction.Type = EDeltaType::NodeReduction;
		nodeDeltaAddition.Type = EDeltaType::NodeAddition;

		for (auto& channel : channels)
		{
			if (channel->bSwitched || channel->Node1->bSwitched || channel->Node2->bSwitched)
			{
				channel->bSwitched = false;

				if (channel->bEnabled && channel->Node1->bEnabled && channel->Node2->bEnabled)
				{
					channelDeltaAddition.AffectedNodes.push_back(channel->Node1->ID);
					channelDeltaAddition.AffectedNodes.push_back(channel->Node2->ID);
				}
				else
				{
					channelDeltaReduction.AffectedNodes.push_back(channel->Node1->ID);
					channelDeltaReduction.AffectedNodes.push_back(channel->Node2->ID);
				}

			}

			if (channel->bWeightChanged)
			{
				channel->bWeightChanged = false;
				channelDeltaReduction.AffectedNodes.push_back(channel->Node1->ID);
				channelDeltaReduction.AffectedNodes.push_back(channel->Node2->ID);
			}
		}


		for (auto& node : nodes)
		{
			if (node->bSwitched)
			{
				node->bSwitched = false;
				if (node->bEnabled)
					nodeDeltaAddition.AffectedNodes.push_back(node->ID);
				else
					nodeDeltaReduction.AffectedNodes.push_back(node->ID);
			}
		}

		PropagateDelta(nodes, channelDeltaReduction);
		PropagateDelta(nodes, channelDeltaAddition);
		PropagateDelta(nodes, nodeDeltaReduction);
		PropagateDelta(nodes, nodeDeltaAddition);

	}

}

