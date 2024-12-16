#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include <vector>




namespace VNR
{
	// TODO: Separate TopologyData and TopologySettings to different files
	struct TopologyData
	{
		std::vector<int32> Weights;
		int32 RegionalNetworks = 0;
		int32 NodesInNetwork = 0;
		int32 AVGNetworkDegree = 0;
	};

	DECLARE_DELEGATE_ONE_PARAM(GenerateGraphDelegate, TopologyData)


	class TopologySettigs
	{
	public:
		TopologySettigs();
		
		void InitTopologyData(TopologyData data);				   
		TopologyData GetTopologyData() const;

		void InitWeights(const std::vector<int32>& weights);
		void InitRegionalNetworks(uint32 regionalNetworks);
		void InitNodesInNetwork(uint32 nodes);
		void InitAVGNetworkDegree(float degree);

		std::vector<int32> GetWeights() const;
		int32 GetRegionalNetworks() const;
		int32 GetNodesInNetwork() const;
		float GetAVGNetworkDegree() const;
		

		void Draw();
		void WeightsSection();
		void NodesSection();
		void NetworkDegreeSection();
	public:
		GenerateGraphDelegate OnGenerate;

	private:
		TopologyData m_TopologyData;
		float m_AVGNetworkDegree; 
	};
}