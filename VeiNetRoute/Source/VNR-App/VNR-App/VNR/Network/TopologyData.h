#pragma once

#include <vector>

namespace VNR
{
	struct TopologyData
	{
		std::vector<int32> Weights;
		std::vector<int32> NodesInNetwork;
		float AVGNetworkDegree = 0;
	};
}