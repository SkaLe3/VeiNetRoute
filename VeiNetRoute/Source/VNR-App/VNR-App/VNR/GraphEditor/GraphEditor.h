#pragma once

#include "VNR-App/VNR/GraphEditor/Canvas.h"
#include "VNR-App/VNR/GraphEditor/Node.h"
#include "VNR-App/VNR/GraphEditor/Edge.h"

#include "VNR-App/VNR/Network/Channel.h"
#include "VNR-App/VNR/Network/TopologyData.h"

#include <vector>
#include <unordered_map>

namespace VNR
{
	class GraphEditor
	{
	public:
		GraphEditor();

		
		void GenerateGraph(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels, TopologyData topology);
		void AddEdge(Channel* channel);
		void AddNode(NetworkNode* node);
		void RemoveEdge(Edge* edge);
		void RemoveNode(Node* node);

		void Draw();

		Node* GetSelectedNode() const;

		static std::unordered_map<EChannelType, ImU32> EdgeColors;
	private:
		void Pan();
		void Zoom();
		void SelectNode();
		void DragNode();

		void DrawNode(const UniquePtr<Node>& node);
		void DrawEdge(const UniquePtr<Edge>& edge);

		void ArrangeNodes();

		bool AreInSameRegionalNetwork(const UniquePtr<Node>& node1, const UniquePtr<Node>& node2 );
		bool AreInSameRegionalNetwork(const Node* node1, const Node* node2);


	private:
		Canvas m_Canvas;

		/* Settings */
		float m_ZoomSpeed = 0.1f;

		/* Parameters */
		const float m_EdgeNumberOffset = 17.f;

		/* Data */
		std::vector<UniquePtr<Node>> m_Nodes;
		std::vector<UniquePtr<Edge>> m_Edges;

		TopologyData m_Topology;

		Node* m_SelectedNode;

		
	};
}