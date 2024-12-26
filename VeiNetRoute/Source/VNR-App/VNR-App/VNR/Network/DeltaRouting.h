#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <vector>
#include <unordered_map>

namespace VNR
{
	enum class EDeltaType : uint8
	{
		ChannelReduction = 0,
		ChannelAddition,
		NodeReduction,
		NodeAddition,
	};

	struct Route
	{
		int32 DestinationID;
		int32 NextHopID;
		int32 Cost;
	};

	struct Delta
	{
		EDeltaType Type;
		std::vector<int32> AffectedNodes;
	};


	using NRoutingTable = std::unordered_map<int32, Route>;


	class NetworkNode;


	namespace DeltaRouting
	{
		void SynchronizeRoutingTables(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);
		void PropagateDelta(std::vector<UniquePtr<NetworkNode>>& nodes, const Delta& delta);

		void ValidateTopologyChanges(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);

		void InvalidateChangedRoutes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32>& ids);
		void InvalidateDestinationNode(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32>& ids);
		void InitializeRoutes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32> ids);
		void InitializeNodes(std::vector<UniquePtr<NetworkNode>>& nodes, const std::vector<int32> ids);

	}

}