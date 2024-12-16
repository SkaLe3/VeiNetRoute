#pragma once
#include "VNR-Core/Core/CoreDefines.h"

namespace VNR
{
	struct NetworkNode;

	struct Channel
	{
	   NetworkNode* Node1;
	   NetworkNode* Node2;

	   int32 Weight;

	   Channel(NetworkNode* node1, NetworkNode* node2, int32 weight) : Node1(node1), Node2(node2), Weight(weight) {}
	};
}