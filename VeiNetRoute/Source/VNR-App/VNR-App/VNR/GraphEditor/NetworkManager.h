#pragma once
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"

namespace VNR
{
	DECLARE_DELEGATE_ONE_PARAM(CreateChannelDelegate, ChannelData)
	DECLARE_DELEGATE_ONE_PARAM(CreateNodeDelegate, NetworkNodeData)
	DECLARE_DELEGATE_ONE_PARAM(DestroyChannelDelegate, Channel*)
	DECLARE_DELEGATE_ONE_PARAM(DestroyNodeDelegate, NetworkNode*)

	class NetworkManager
	{
	public:
		NetworkManager();

		void ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);

		void Draw();

	public:
		CreateChannelDelegate OnCreateChannel;
		CreateNodeDelegate OnCreateNode;
		DestroyChannelDelegate OnDestroyChannel;
		DestroyNodeDelegate	OnDestroyNode;

	private:
		void AddChannelPopup();
		void AddNodePopup();

		void ConnectionTab();
		void NodeTab();

		bool Contains(int32 ID);

	private:
		std::vector<UniquePtr<NetworkNode>>* m_Nodes;
		std::vector<UniquePtr<Channel>>* m_Channels;

		ChannelData m_NewChannel;
		NetworkNodeData m_NewNode;

	};
}