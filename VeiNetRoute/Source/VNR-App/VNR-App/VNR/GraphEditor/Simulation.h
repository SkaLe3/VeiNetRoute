#pragma once
#include "VNR-Core/Core/CoreDefines.h"

// For PacketTransmission
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/NetworkProtocol.h"

namespace VNR
{
	struct PacketTransmissionResult
	{
		ENetworkProtocol Protocol;
		NetworkNode* StartNode;
		NetworkNode* EndNode;
		int32 MessageSize;
		int32 PacketCount;
		int32 ServiceSize;
		int32 PayloadSize;
		int32 ServiceCount;
		int32 PayloadCount;
		int32 TimeSpent;

	};

	class Simulation
	{
	public:
		
		void Draw();

	private:
	};
}