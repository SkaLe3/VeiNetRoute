#include "Network.h"

#include <algorithm>
#include <limits>
#include <iostream>

#include <glm/glm.hpp>

namespace VNR
{

	Network::Network(TopologyData data)
		: m_TopologyData(data)
	{
		uint32 counter = 0;
		for (uint32 i = 0; i < data.NodesInNetwork.size(); i++)
		{
			for (uint32 j = 0; j < data.NodesInNetwork[i]; j++)
			{
				AddNode({ (int32)counter, true });
				counter++;
			}
		}
		GenerateNetwork();
	}

	void Network::GenerateNetwork()
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		// Early return on empty weights
		if (m_TopologyData.Weights.empty()) return;

		std::uniform_int_distribution<> weightDis(0, m_TopologyData.Weights.size() - 1);
		std::uniform_int_distribution<> errorDis(m_TopologyData.ErrorRange[0], m_TopologyData.ErrorRange[1]);
		std::uniform_int_distribution<> typeDis(0, 1);
		std::vector<std::pair<int32, int32>> connections;

		// Calculate total number of connections and distribute them among subnetworks
		int32 networkCount = m_TopologyData.NodesInNetwork.size();
		int32 totalConnections = glm::ceil(Nodes.size() * m_TopologyData.AVGNetworkDegree * 0.5f) - (networkCount - 1);

		std::vector<int32> subNetworkConnectionLimits;
		int32 usedConnections = 0;
		for (int32 subNetworkIdx = 0; subNetworkIdx < networkCount; subNetworkIdx++)
		{
			float coeficient = (float)m_TopologyData.NodesInNetwork[subNetworkIdx] / (float)Nodes.size();
			int32 neededConnections = (float)totalConnections * coeficient;
			usedConnections += neededConnections;
			subNetworkConnectionLimits.push_back(neededConnections);
		}
		int32 extraConnections = totalConnections - usedConnections;
		for (int32 i = 0; i < extraConnections; ++i)
		{
			subNetworkConnectionLimits[i]++;
		}

		int32 startNodeIndex = 0;
		int32 startConnectionIndex = 0;
		// Iterate through each subnetwork
		for (int32 subNetworkIdx = 0; subNetworkIdx < networkCount; subNetworkIdx++)
		{
			int32 subNetworkSize = m_TopologyData.NodesInNetwork[subNetworkIdx];
			std::vector<int32> nodeIndices;

			// Populate node indices for the current subnetwork
			for (uint32 i = 0; i < subNetworkSize; i++)
				nodeIndices.push_back(startNodeIndex + i);

			std::uniform_int_distribution<> nodesDis(0, nodeIndices.size() - 1);

			int32 firstNode = nodeIndices[nodesDis(gen)];
			int32 firstNodeBackup = firstNode;
			nodeIndices.erase(std::find(nodeIndices.begin(), nodeIndices.end(), firstNode));

			// Connect all nodes in random order in one line
			while (!nodeIndices.empty())
			{
				std::uniform_int_distribution<> nodesDis(0, nodeIndices.size() - 1);
				int32 secondNode = nodeIndices[nodesDis(gen)];
				connections.push_back(std::make_pair<>(firstNode, secondNode));
				firstNode = secondNode;
				nodeIndices.erase(std::find(nodeIndices.begin(), nodeIndices.end(), secondNode));
			}

			int32 remainingConnections = subNetworkConnectionLimits[subNetworkIdx] - subNetworkSize + 1;

			// Populate node indices for the current subnetwork
			for (uint32 degree = 0; degree < m_TopologyData.AVGNetworkDegree - 2; degree++)
				for (uint32 i = 0; i < subNetworkSize; i++)
					nodeIndices.push_back(startNodeIndex + i);

			nodeIndices.push_back(firstNodeBackup);
			nodeIndices.push_back(firstNode);

			// Add remaining connections
			for (uint32 i = 0; i < remainingConnections; i++)
			{
				std::uniform_int_distribution<> nodesDis(0, nodeIndices.size() - 1);

				// Select random nodes, but not the same
				uint32 rndIdx1 = nodesDis(gen);
				uint32 rndIdx2 = nodesDis(gen);
				while (rndIdx1 == rndIdx2) { rndIdx2 = nodesDis(gen); }
				uint32 node1 = nodeIndices[rndIdx1];
				uint32 node2 = nodeIndices[rndIdx2];

				// If connection cannot be created, trying to shuffle with previous connection
				while (node1 == node2 || std::any_of(connections.begin(), connections.end(), [&](const std::pair<int32, int32>& con)
													 {
														 return con.first == node1 && con.second == node2 || con.first == node2 && con.second == node1;
													 }))
				{

					std::pair<int32, int32> con = *std::find_if(connections.begin() + startConnectionIndex, connections.end(), [&](const std::pair<int32, int32>& con)
																{
																	return con.first != node1 && con.second != node2 && con.first != node2 && con.second != node1;
																});
					// Erase selected connection, to put in the back, so it won't stuck shuffling the same 2 connections 
					connections.erase(std::find(connections.begin(), connections.end(), con));

					// Swap first nodes 
					int32 temp = con.first;
					con.first = node1;
					node1 = temp;

					connections.push_back(con);

					// Validate remaining node indices vector
					nodeIndices.erase(std::find(nodeIndices.begin(), nodeIndices.end(), con.first));
					nodeIndices.push_back(temp);
				}
				// Erase used nodes
				nodeIndices.erase(std::find(nodeIndices.begin(), nodeIndices.end(), node1));
				nodeIndices.erase(std::find(nodeIndices.begin(), nodeIndices.end(), node2));

				connections.emplace_back(node1, node2);
			}
			startNodeIndex += subNetworkSize;
			startConnectionIndex = connections.size();
		}

		// Connect subnetworks
		startNodeIndex = 0;
		for (int32 subNetworkIdx = 0; subNetworkIdx < networkCount - 1; subNetworkIdx++)
		{
			startNodeIndex += m_TopologyData.NodesInNetwork[subNetworkIdx];
			connections.emplace_back(startNodeIndex - 1, startNodeIndex);
		}

		// Create Channels based on connections
		int32 channelIdx = 0;
		for (const auto& [node1, node2] : connections)
		{
			int32 weight = m_TopologyData.bRandomWeights ? m_TopologyData.Weights[weightDis(gen)] : m_TopologyData.Weights[channelIdx % m_TopologyData.Weights.size()];
			float errorValue = errorDis(gen) * 0.01f;
			EChannelType channelType = ToEChannelType(typeDis(gen));

			AddChannel({ {node1, node2}, channelType, weight, errorValue });
			channelIdx++;
		}

		InitRoutingTables();
	}

	Channel* Network::AddChannel(ChannelData data)
	{
		NetworkNode* nodes[2];

		for (int32 i = 0; i < 2; i++)
		{
			auto nodeIt = std::find_if(Nodes.begin(), Nodes.end(),
									   [&data, &i](const UniquePtr<NetworkNode>& nodePtr)
									   {
										   return nodePtr->ID == data.NodesID[i];
									   }
			);
			if (nodeIt != Nodes.end())
			{
				nodes[i] = nodeIt->get();
			}
			else
			{
				return nullptr;
			}
		}
		Channels.push_back(MakeUnique<Channel>(nodes[0], nodes[1], data.Type, data.Weight, data.ErrorProbability));
		Channel* channel = Channels.back().get();
		nodes[0]->Channels.push_back(channel);
		nodes[1]->Channels.push_back(channel);

		if (!m_bRoutingTablesInitialized)
			return channel;

		DeltaRouting::PropagateDelta(Nodes, { EDeltaType::ChannelAddition, {channel->Node1->ID, channel->Node2->ID }});
		DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);

		return channel;
	}

	NetworkNode* Network::AddNode(NetworkNodeData data)
	{
		Nodes.push_back(MakeUnique<NetworkNode>(data.ID, data.bEnabled));
		NetworkNode* node = Nodes.back().get();

		DeltaRouting::PropagateDelta(Nodes, { EDeltaType::NodeAddition, {node->ID} });
		DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);
		return node;
	}

	void Network::RemoveChannel(Channel* channel)
	{
		NetworkNode* node1 = channel->Node1;
		NetworkNode* node2 = channel->Node2;

		std::vector<Channel*>& node1Channels = node1->Channels;
		if (!node1Channels.empty())
		{
			auto node1channelIt = std::find_if(node1Channels.begin(), node1Channels.end(),
											   [&channel](const Channel* ptr)
											   {
												   return ptr == channel;
											   });
			if (node1channelIt != node1Channels.end())
			{
				node1Channels.erase(node1channelIt);
			}
		}

		std::vector<Channel*>& node2Channels = node2->Channels;
		if (!node2Channels.empty())
		{
			auto node2channelIt = std::find_if(node2Channels.begin(), node2Channels.end(),
											   [&channel](const Channel* ptr)
											   {
												   return ptr == channel;
											   });
			if (node2channelIt != node2Channels.end())
			{
				node2Channels.erase(node2channelIt);
			}
		}


		auto it = std::find_if(Channels.begin(), Channels.end(),
							   [&channel](const UniquePtr<Channel>& ptr)
							   {
								   return ptr.get() == channel;
							   });

		if (it != Channels.end())
		{
			Channels.erase(it);
		}

		if (!m_bRoutingTablesInitialized)
			return;

		DeltaRouting::PropagateDelta(Nodes, { EDeltaType::ChannelReduction, {node1->ID, node2->ID} });
		DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);
	}

	void Network::RemoveNode(NetworkNode* node)
	{
		int32 nodeID = node->ID;
		auto it = std::find_if(Nodes.begin(), Nodes.end(),
							   [&node](const UniquePtr<NetworkNode>& ptr)
							   {
								   return ptr.get() == node;
							   });

		if (it != Nodes.end())
		{
			Nodes.erase(it);
		}

		if (!m_bRoutingTablesInitialized)
			return;

		DeltaRouting::PropagateDelta(Nodes, { EDeltaType::NodeReduction, {nodeID} });
		DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);
	}

	TopologyData Network::GetTopology() const
	{
		return m_TopologyData;
	}



	void Network::SimulationStep(float deltaTime)
	{
		static float timeElapsed = 0;
		timeElapsed += deltaTime;
		if (timeElapsed > SynchronizationTimer)
		{
			DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);
			timeElapsed = 0;
			VNR_TRACE("Network Routing tables Synchronized!");
		}

	}

	void Network::SetSynchronizationTimer(float delta)
	{
		SynchronizationTimer = delta;
	}

	std::unordered_map<int32, std::vector<Route>> Network::GetAllRoutingTables()
	{
		std::unordered_map<int32, std::vector<Route>> routingTables;

		for (const auto& node : Nodes)
		{
			std::vector<Route> routeTable;
			for (const auto& [id, route] : node->RoutingTable)
			{
				routeTable.push_back(route);
			}

			routingTables[node->ID] = routeTable;
		}
		return routingTables;
	}

	void Network::InitRoutingTables()
	{
		m_bRoutingTablesInitialized = true;


		for (auto& node : Nodes)
		{
			for (auto& destinationNode : Nodes)
			{
				node->RoutingTable[destinationNode->ID] = { destinationNode->ID, -1,  std::numeric_limits<int32>::max() };
			}
			node->RoutingTable[node->ID] = { node->ID, node->ID, 0 };

		}
		DeltaRouting::SynchronizeRoutingTables(Nodes, Channels);

	}

}

