#pragma once

#include <vector>

namespace VNR
{
	struct TopologyData
	{
		std::vector<int32> Weights;
		std::vector<int32> NodesInNetwork;
		float AVGNetworkDegree = 0;
		int32 ErrorRange[2] = {0, 30};
		bool bRandomWeights = true;
	};
}