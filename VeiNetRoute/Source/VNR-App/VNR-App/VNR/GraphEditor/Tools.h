#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <filesystem>

namespace VNR
{
	class VNRLayer;


	class Tools
	{
	public:
		Tools(VNRLayer* layer);
		void ProvideNetworkData(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);

		void Draw();


	private:
		void ExportRoutingTablesToCSV(const std::filesystem::path& filename);

		void NodeSelector(const char* label, int32* location, int32 width);
		NetworkNode* FindNode(int32 nodeID);
		Channel* FindChannel(NetworkNode* node1, int32 node2);
		void ClearHighlight();

		std::vector<Channel*> FindFewestHopsPath(NetworkNode* source, NetworkNode* destination);

	private:
		VNRLayer* m_AppLayer;

		std::vector<UniquePtr<NetworkNode>>* m_Nodes;
		std::vector<UniquePtr<Channel>>* m_Channels;

		std::filesystem::path m_RoutingDataPath;

		int32 m_SourceNodeID = -1;
		int32 m_DestinationNodeID = -1;
	};
}