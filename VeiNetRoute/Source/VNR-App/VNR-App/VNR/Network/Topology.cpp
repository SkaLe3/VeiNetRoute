#include "Topology.h"

#include <imgui.h>
#include <algorithm>
#include <string>

namespace VNR
{

	TopologySettigs::TopologySettigs()
		: m_AVGNetworkDegree(2.5)
	{

	}

	void TopologySettigs::InitTopologyData(TopologyData data)
	{
		m_TopologyData = data;
	}

	TopologyData TopologySettigs::GetTopologyData() const
	{
		return m_TopologyData;
	}

	void TopologySettigs::InitWeights(const std::vector<int32>& weights)
	{
		m_TopologyData.Weights = weights;
	}

	std::vector<int32> TopologySettigs::GetWeights() const
	{
		return m_TopologyData.Weights;
	}

	void TopologySettigs::InitRegionalNetworks(uint32 regionalNetworks)
	{
		m_TopologyData.RegionalNetworks = regionalNetworks;
	}

	void TopologySettigs::InitNodesInNetwork(uint32 nodes)
	{
		m_TopologyData.NodesInNetwork = nodes;
	}

	void TopologySettigs::InitAVGNetworkDegree(float degree)
	{
		m_TopologyData.AVGNetworkDegree = degree;
	}

	int32 TopologySettigs::GetRegionalNetworks() const
	{
		return m_TopologyData.RegionalNetworks;
	}

	int32 TopologySettigs::GetNodesInNetwork() const
	{
		return m_TopologyData.NodesInNetwork;
	}

	float TopologySettigs::GetAVGNetworkDegree() const
	{
		return m_TopologyData.AVGNetworkDegree;
	}

	void TopologySettigs::Draw()
	{

		WeightsSection();
		NodesSection();
		NetworkDegreeSection();

		ImGui::Separator();
		ImGui::Spacing();
		float windowWidth = ImGui::GetContentRegionAvail().x;
		ImFont* customFont = ImGui::GetIO().Fonts->Fonts[0];

		ImGui::PushFont(customFont);
		if (ImGui::Button("Generate", ImVec2(windowWidth, 30)))
		{
			m_TopologyData.AVGNetworkDegree = (int32)(m_AVGNetworkDegree / 2.f);
			OnGenerate.Broadcast(m_TopologyData);
		}
		ImGui::PopFont();

	}

	void TopologySettigs::WeightsSection()
	{
		ImFont* customFont = ImGui::GetIO().Fonts->Fonts[0];
		ImGui::PushFont(customFont);
		if (!ImGui::CollapsingHeader("Weights"))
		{
			ImGui::PopFont();
			return;
		}
		ImGui::PopFont();

		ImGui::Spacing();
		float windowWidth = ImGui::GetContentRegionAvail().x;
		if (ImGui::Button("Add Weight", ImVec2(windowWidth, 0)))
		{
			m_TopologyData.Weights.push_back(0);
		}
		ImGui::Spacing();
		int32 removeIndex = -1;
		for (size_t i = 0; i < m_TopologyData.Weights.size(); i++)
		{
			ImGui::PushItemWidth(100);
			if (ImGui::InputInt(("##Weight" + std::to_string(i + 1)).c_str(), &m_TopologyData.Weights[i], 1, 1))
			{
				m_TopologyData.Weights[i] = std::clamp(m_TopologyData.Weights[i], 0, 100);
			}
			ImGui::PopItemWidth();
			ImGui::SameLine();

			if (ImGui::Button(("Remove##" + std::to_string(i)).c_str()))
			{
				if (m_TopologyData.Weights.size() > 1) // Do not delete the last element
					removeIndex = i;
			}
		}
		if (removeIndex != -1)
		{
			m_TopologyData.Weights.erase(m_TopologyData.Weights.begin() + removeIndex);
		}
		ImGui::Spacing();
	}

	void TopologySettigs::NodesSection()
	{
		ImFont* customFont = ImGui::GetIO().Fonts->Fonts[0];
		ImGui::PushFont(customFont);
		if (!ImGui::CollapsingHeader("Nodes"))
		{
			ImGui::PopFont();
			return;
		}
		ImGui::PopFont();

		ImGui::PushItemWidth(100);
		if (ImGui::InputInt("Regional Networks", &m_TopologyData.RegionalNetworks))
		{
			m_TopologyData.RegionalNetworks = std::clamp(m_TopologyData.RegionalNetworks, 0, 5);
		}
		if (ImGui::InputInt("Nodes in Network", &m_TopologyData.NodesInNetwork))
		{
			m_TopologyData.NodesInNetwork = std::clamp(m_TopologyData.NodesInNetwork, 0, 30);
		}
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}

	void TopologySettigs::NetworkDegreeSection()
	{
		ImFont* customFont = ImGui::GetIO().Fonts->Fonts[0];
		ImGui::PushFont(customFont);
		if (!ImGui::CollapsingHeader("NetworkDegree"))
		{
			ImGui::PopFont();
			return;
		}
		ImGui::PopFont();

		ImGui::PushItemWidth(100);
		if (ImGui::InputFloat("Average Network Degree", &m_AVGNetworkDegree, 0.5f, 1.f, "%.1f"))
		{
			m_AVGNetworkDegree = std::clamp(m_AVGNetworkDegree, 0.f, 5.f);
		}
		ImGui::PopItemWidth();
		ImGui::Spacing();
	}

}