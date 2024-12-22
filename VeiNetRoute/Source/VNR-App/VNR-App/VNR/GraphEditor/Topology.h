#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include "VNR-App/VNR/Network/TopologyData.h"

#include <vector>




namespace VNR
{
	DECLARE_DELEGATE_ONE_PARAM(GenerateGraphDelegate, TopologyData)


	class TopologySettigs
	{
	public:
		TopologySettigs();

		void InitTopologyData(TopologyData data);
		TopologyData GetTopologyData() const;

		void InitWeights(const std::vector<int32>& weights);
		void InitRegionalNetworks(const std::vector<int32>& regionalNetworks);
		void InitAVGNetworkDegree(float degree);

		std::vector<int32> GetWeights() const;
		std::vector<int32> GetRegionalNetworks() const;
		float GetAVGNetworkDegree() const;


		void Draw();
		void WeightsSection();
		void NodesSection();
		void NetworkDegreeSection();
		void ErrorSection();
	public:
		GenerateGraphDelegate OnGenerate;

	private:
		TopologyData m_TopologyData;
		float m_AVGNetworkDegree;
	};
}