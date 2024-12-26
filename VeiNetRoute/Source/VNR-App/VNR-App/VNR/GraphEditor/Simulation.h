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
		int32 SourceNode;
		int32 DestinationNode;
		int32 MessageSize;
		int32 PacketCount;
		int32 ServiceSize;
		int32 PayloadSize;
		int32 TransmissionTime;

		int32 RetransmissionCount;

		float PacketsLossRate;

		PacketTransmissionResult()
			: Protocol(ENetworkProtocol::UDP)
			, SourceNode(-1)
			, DestinationNode(-1)
			, MessageSize(0)
			, PacketCount(0)
			, ServiceSize(0)
			, PayloadSize(0)
			, TransmissionTime(0)
			, RetransmissionCount(0)
			, PacketsLossRate(0.0f)
		{}
	};

	struct PacketTransmissionSettings
	{
		int32 SourceNode = -1;
		int32 DestinationNode = -1;
		ENetworkProtocol Protocol = ENetworkProtocol::UDP;
		int32 MessageCount = -1;
		int32 MessageSize = -1;
		int32 MTU = 1500;


		bool bRandomSourceNode = true;
		bool bRandomDestinationNode = true;
		bool bRandomProtocol = true;
		bool bRandomMessageCount = true;
		bool bRandomMessageSize = true;
		bool bRandomMTU = false;
	};	

	class Simulation
	{
	public:
		Simulation();
		void ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);
		
		void Draw();

	private:
		void ControlPanel();
		void SimulationTable();

		void NodeSelector(const char* label, int32* location, bool &random, int32 width);

	private:
		std::vector<UniquePtr<NetworkNode>>* m_Nodes;
		std::vector<UniquePtr<Channel>>* m_Channels;

		int32 m_SourceID = -1;
		int32 m_DestinationID = -1;

		PacketTransmissionSettings m_Settings;
		std::vector<PacketTransmissionResult> m_Result;
	};
}