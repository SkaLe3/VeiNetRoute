#pragma once
#include "VNR-Core/Core/CoreDefines.h"

namespace VNR
{
	struct NetworkNode;
	struct Edge;

	enum class EChannelType : uint8
	{
		Duplex = 0,
		HalfDuplex
	};

	ENUM_CLASS_CONVERSIONS(EChannelType)

	struct ChannelData
	{
		int32 NodesID[2] = { -1, -1 };
		EChannelType Type = EChannelType::Duplex;
		int32 Weight = 0;
		float ErrorProbability = 0;
	};

	struct Channel
	{
		NetworkNode* Node1;
		NetworkNode* Node2;

		EChannelType Type;

		int32 Weight;
		float ErrorProbability;

		Edge* Visuals;

		Channel(NetworkNode* node1, NetworkNode* node2, EChannelType type, int32 weight, float ep)
			: Node1(node1), Node2(node2), Type(type), Weight(weight), ErrorProbability(ep)
		{}

	};
}