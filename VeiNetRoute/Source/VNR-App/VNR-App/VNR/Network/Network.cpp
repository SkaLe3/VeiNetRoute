#include "Network.h"

#include <random>

namespace VNR
{

	Network::Network(TopologyData data)
		: m_TopologyData(data)
	{
		for (uint32 i = 0; i < data.NodesInNetwork * data.RegionalNetworks; i++)
		{
			Nodes.push_back(MakeUnique<NetworkNode>(i, ENodeType::Type1));
		}
		GenerateNetwork();
	}

	void Network::GenerateNetwork()
	{
		std::random_device rd;
		std::mt19937 gen(rd());

		if (m_TopologyData.Weights.empty())
		{
			m_TopologyData.Weights.push_back(0);
		}

		for (const auto& node : Nodes)
		{
			int32 edgesNeeded = m_TopologyData.AVGNetworkDegree;
			std::vector<NetworkNode*> neighbors;
			for (const auto& node : Nodes)
			{
				neighbors.push_back(node.get());
			}
			neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), node.get()), neighbors.end());

			for (int32 i = 0; i < edgesNeeded; i++)
			{
				if (!neighbors.empty())
				{
					std::uniform_int_distribution<> neighborDis(0, neighbors.size() - 1);
					std::uniform_int_distribution<> weightDis(0, m_TopologyData.Weights.size() - 1);
					NetworkNode* neighbor = neighbors[neighborDis(gen)];
					int32 weight = m_TopologyData.Weights[weightDis(gen)];

					neighbors.erase(std::remove(neighbors.begin(), neighbors.end(), neighbor), neighbors.end());

					Channels.push_back(MakeUnique<Channel>(node.get(), neighbor, weight));
					Channel* channel = Channels.back().get();
					node->Channels.push_back(channel);
					neighbor->Channels.push_back(channel);

				}
			}
		}
	}

}

