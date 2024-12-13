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
		void Draw();


	private:
		void Pan();
		void Zoom();
		void SelectNode();
		void DragNode();

		void DrawNode(const Node& node);
		void DrawEdge(const Edge& edge);

	private:
		Canvas m_Canvas;

		/* Settings */
		float m_ZoomSpeed = 0.1f;

		/* Data */
		std::vector<Node> m_Nodes;
		std::vector<Edge> m_Edges;
	};
}