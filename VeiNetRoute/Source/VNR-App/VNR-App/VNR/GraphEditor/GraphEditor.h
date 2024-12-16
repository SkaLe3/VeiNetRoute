#pragma once

#include "VNR-App/VNR/GraphEditor/Canvas.h"
#include "VNR-App/VNR/GraphEditor/Node.h"
#include "VNR-App/VNR/GraphEditor/Edge.h"

#include <vector>

namespace VNR
{
	class GraphEditor
	{
	public:
		GraphEditor();

		
		void GenerateGraph(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels);
		void Draw();
	private:
		void Pan();
		void Zoom();
		void SelectNode();
		void DragNode();

		void DrawNode(const UniquePtr<Node>& node);
		void DrawEdge(const UniquePtr<Edge>& edge);

	private:
		Canvas m_Canvas;

		/* Settings */
		float m_ZoomSpeed = 0.1f;

		/* Parameters */
		const float m_EdgeNumberOffset = 17.f;

		/* Data */
		std::vector<UniquePtr<Node>> m_Nodes;
		std::vector<UniquePtr<Edge>> m_Edges;

		
	};
}