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
		int32 PacketsSent;
		int32 PacketsReceived;
		int32 ServiceSize;
		float TransmissionTime;

		int32 RetransmissionCount;

		float PacketsLossRate;

		PacketTransmissionResult()
			: Protocol(ENetworkProtocol::UDP)
			, SourceNode(-1)
			, DestinationNode(-1)
			, MessageSize(0)
			, PacketsSent(0)
			, PacketsReceived(0)
			, ServiceSize(0)
			, TransmissionTime(0.f)
			, RetransmissionCount(0)
			, PacketsLossRate(0.0f)
		{}
		PacketTransmissionResult(ENetworkProtocol protocol, int32 s, int32 d, int32 ms, int32 psc, int32 prc, int32 ss, float tt, int32 rc, float plr)
		: Protocol(protocol), SourceNode(s), DestinationNode(d), MessageSize(ms), PacketsSent(psc), PacketsReceived(prc), ServiceSize(ss),
		 TransmissionTime(tt), RetransmissionCount(rc), PacketsLossRate(plr)  {}
	};

	struct PacketTransmissionSettings
	{
		int32 SourceNode = -1;
		int32 DestinationNode = -1;
		ENetworkProtocol Protocol = ENetworkProtocol::UDP;
		int32 MessageCount = 1;
		int32 MessageSize = 5000;
		int32 MTU = 1500;



		bool bRandomSourceNode = true;
		bool bRandomDestinationNode = true;
		bool bRandomProtocol = true;
		bool bRandomMessageCount = true;
		bool bRandomMessageSize = true;
		bool bRandomMTU = false;
	};
	
	struct PacketTransmissionSpecifications
	{
		int32 SourceNode;
		int32 DestinationNode;
		ENetworkProtocol Protocol;
		int32 MessageSize;
		int32 MTU;

		int32 ServiceSize;
		int32 PayloadSize;

		PacketTransmissionSpecifications(const PacketTransmissionSettings& settings, std::vector<UniquePtr<NetworkNode>>* nodes);
	};

	struct Packet
	{
		ENetworkProtocol Protocol;
		int32 SourceNode;
		int32 DestinationNode;
		int32 ServiceSize;
		int32 PayloadSize;
	};

	struct TransmitInfo
	{
		bool Lost = false;
	};

	class Simulation
	{
	public:
		Simulation();
		void ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);
		
		void Simulate(PacketTransmissionSettings settings);
		bool SendMessage(PacketTransmissionSpecifications& specs);

		void Draw();

	private:
		void ControlPanel();
		void SimulationTable();

		void NodeSelector(const char* label, int32* location, bool &random, int32 width);

		void DisplayDynamicPrecisionPercentage(float value);

		NetworkNode* FindNode(int32 nodeID);
		Channel* FindChannel(NetworkNode* node1, int32 node2);

		bool TransmitPacket(const Packet& packet, NetworkNode* transitionNode, double& time,  float errorOverride = -1.f);

	private:
		std::vector<UniquePtr<NetworkNode>>* m_Nodes;
		std::vector<UniquePtr<Channel>>* m_Channels;

		PacketTransmissionSettings m_Settings;
		std::vector<PacketTransmissionResult> m_Result;
	};
}