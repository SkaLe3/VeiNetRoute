#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/Topology.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <vector>

namespace VNR
{
	class Network
	{
	public:
		Network(TopologyData data);

		void GenerateNetwork();

	public:
		std::vector<UniquePtr<NetworkNode>> Nodes;
		std::vector<UniquePtr<Channel>> Channels;

	private:
		TopologyData m_TopologyData;



	};
}
