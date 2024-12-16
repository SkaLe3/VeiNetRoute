#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <vector>

namespace VNR
{
	struct Node;

	enum class ENodeType
	{
		Type1, Type2, Disabled
	};


	struct NetworkNode
	{
		uint32 ID;
		ENodeType Type;
		std::vector<Channel*> Channels; 

		Node* Visuals;

		NetworkNode(uint32 id, ENodeType type) : ID(id), Type(type)	{}
	};
}