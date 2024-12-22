#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/TopologyData.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <vector>
#include <random>

namespace VNR
{
	class Network
	{
	public:
		Network(TopologyData data);

		void GenerateNetwork();
		Channel* AddChannel(ChannelData data);
		NetworkNode* AddNode(NetworkNodeData data);
		void RemoveChannel(Channel* channel);
		void RemoveNode(NetworkNode* node);


		TopologyData GetTopology() const;


	public:
		std::vector<UniquePtr<NetworkNode>> Nodes;
		std::vector<UniquePtr<Channel>> Channels;

	private:
		TopologyData m_TopologyData;



	};
}
