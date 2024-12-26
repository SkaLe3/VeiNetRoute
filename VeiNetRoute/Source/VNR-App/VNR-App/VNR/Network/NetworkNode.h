#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/Channel.h"
#include "VNR-App/VNR/Network/DeltaRouting.h"

#include <vector>

namespace VNR
{
	struct Node;

	struct NetworkNodeData
	{
		int32 ID = -1;
		bool bEnabled = true;
	};

	

	struct NetworkNode
	{
		int32 ID;
		bool bEnabled;
		std::vector<Channel*> Channels;
		bool bSwitched;

		Node* Visuals;

		NRoutingTable RoutingTable;

		NetworkNode(uint32 id, bool status) : ID(id), bEnabled(status), bSwitched(false) {}
	};
}