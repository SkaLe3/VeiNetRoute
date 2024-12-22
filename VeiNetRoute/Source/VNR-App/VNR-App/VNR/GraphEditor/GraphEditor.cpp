#include "GraphEditor.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"
#include "VNR-App/Utils/Math/MathHelpers.h"

#include <string>
#include <iostream>
#include <algorithm>

#include <cstdlib> 
#include <ctime> 

namespace VNR
{
	std::unordered_map<EChannelType, ImU32> GraphEditor::EdgeColors =
	{
		{EChannelType::Duplex, IM_COL32(165, 140, 40, 255)},
		{EChannelType::HalfDuplex, IM_COL32(165, 40, 40, 255)}
	};




	GraphEditor::GraphEditor()
	{
		m_SelectedNode = nullptr;
		m_Canvas.SetView(m_Canvas.GetView().Origin, 0.5f);
	}

	void GraphEditor::GenerateGraph(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels, TopologyData topology)
	{
		m_Topology = topology;
		m_SelectedNode = nullptr;
		m_Nodes.clear();
		m_Edges.clear();
		for (const auto& node : nodes)
		{
			m_Nodes.push_back(MakeUnique<Node>(node.get()));
			node->Visuals = m_Nodes.back().get();
		}
		for (const auto& channel : channels)
		{
			AddEdge(channel.get());
		}
		ArrangeNodes();
	}

	void GraphEditor::AddEdge(Channel* channel)
	{
		m_Edges.push_back(MakeUnique<Edge>(channel, channel->Node1->Visuals, channel->Node2->Visuals));
		channel->Visuals = m_Edges.back().get();
	}


	void GraphEditor::AddNode(NetworkNode* node)
	{
		m_Nodes.push_back(MakeUnique<Node>(node));
		node->Visuals = m_Nodes.back().get();
	}

	void GraphEditor::RemoveEdge(Edge* edge)
	{
		auto it = std::find_if(m_Edges.begin(), m_Edges.end(),
							   [&edge](const UniquePtr<Edge>& ptr)
							   {
								   return ptr.get() == edge;
							   });

		if (it != m_Edges.end())
		{
			m_Edges.erase(it);
		}
	}

	void GraphEditor::RemoveNode(Node* node)
	{
		if (node == m_SelectedNode)
			m_SelectedNode = nullptr;
		auto it = std::find_if(m_Nodes.begin(), m_Nodes.end(),
							   [&node](const UniquePtr<Node>& ptr)
							   {
								   return ptr.get() == node;
							   });

		if (it != m_Nodes.end())
		{
			m_Nodes.erase(it);
		}
	}

	void GraphEditor::Draw()
	{
		m_Canvas.Begin();

		Pan();
		if (ImGui::IsWindowHovered())
		{
			Zoom();
			SelectNode();
			DragNode();
		}
		for (const auto& edge : m_Edges)
		{
			DrawEdge(edge);
		}
		for (const auto& node : m_Nodes)
		{
			DrawNode(node);
		}

		m_Canvas.End();
	}

	Node* GraphEditor::GetSelectedNode() const
	{
		return m_SelectedNode;
	}

	void GraphEditor::Pan()
	{
		ImGuiIO& io = ImGui::GetIO();
		CanvasView originalView = m_Canvas.GetView();
		static ImVec2 lastMousePos = io.MousePos;
		static bool clickedOnHover = false;

		if (ImGui::IsMouseDown(1) && ImGui::IsWindowHovered())
			clickedOnHover = true;

		if (ImGui::IsMouseReleased(1))
			clickedOnHover = false;

		if (ImGui::IsMouseDown(1) && clickedOnHover)
		{
			ImVec2 delta;
			delta = io.MousePos - lastMousePos;
			m_Canvas.SetView(originalView.Origin + delta * originalView.Scale, originalView.Scale);
		}
		else
		{
			lastMousePos = io.MousePos;
		}
	}

	void GraphEditor::Zoom()
	{
		ImGuiIO& io = ImGui::GetIO();
		CanvasView originalView = m_Canvas.GetView();
		ImVec2 globalMouseClipped = m_Canvas.FromLocal(io.MousePos) - m_Canvas.GetPos();

		if (io.MouseWheel == 0.0f) return;

		float zoom = originalView.Scale;
		if (io.MouseWheel > 0)
			zoom += m_ZoomSpeed;
		else if (io.MouseWheel < 0)
			zoom -= m_ZoomSpeed;
		zoom = ImClamp(zoom, 0.1f, 3.0f);

		ImVec2 offset = globalMouseClipped - originalView.Origin;
		offset = offset * (zoom - originalView.Scale) / originalView.Scale;
		m_Canvas.SetView(originalView.Origin - offset, zoom);
	}

	void GraphEditor::SelectNode()
	{
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMouseClicked(0))
		{
			bool anyNodeSelected = false;

			for (auto it = m_Nodes.rbegin(); it != m_Nodes.rend(); ++it)
			{
				auto& node = *it;
				if (ImLengthSqr(io.MousePos - node->Position) <= (node->Radius * node->Radius) && !anyNodeSelected)
				{
					node->bSelected = true;
					anyNodeSelected = true;
					m_SelectedNode = node.get();
				}
				else
				{
					node->bSelected = false;
				}
			}
			if (!anyNodeSelected)
				m_SelectedNode = nullptr;
		}
	}

	void GraphEditor::DragNode()
	{
		ImGuiIO& io = ImGui::GetIO();
		bool anyNodeDraggedThisFrame = false;
		for (auto it = m_Nodes.rbegin(); it != m_Nodes.rend(); ++it)
		{
			auto& node = *it;

			if (ImGui::IsMouseClicked(0))
			{
				if (ImLengthSqr(io.MousePos - node->Position) <= (node->Radius * node->Radius) && !anyNodeDraggedThisFrame)
				{
					node->bDragging = true;
					anyNodeDraggedThisFrame = true;
					node->DragOffset = io.MousePos - node->Position;
				}
			}

			if (ImGui::IsMouseReleased(0))
			{
				node->bDragging = false;
			}

			if (node->bDragging)
			{
				node->Position = io.MousePos - node->DragOffset;
			}
		}


	}

	void GraphEditor::DrawNode(const UniquePtr<Node>& node)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();
		ImFont* nodeFont = ImGui::GetIO().Fonts->Fonts[1];
		CanvasView view = m_Canvas.GetView();

		ImVec2 position = m_Canvas.FromLocal(node->Position);
		float radius = m_Canvas.ApplyScale(node->Radius);

		std::string text = std::to_string(node->Data->ID);
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		ImVec2 textPos = m_Canvas.FromLocal(node->Position - ImGui::CalcTextSize(text.c_str()) * 1.5f);

		ImU32 outlineColor = node->Data->bEnabled ? Node::OutlineColor : Node::DisabledOutlineColor;
		ImU32 displayedOutlineColor = node->bSelected ? Node::SelectedColor : outlineColor;

		float outlineThickness = node->bSelected ?
			m_Canvas.ApplyScale(node->m_OutlineThicknessSelected) : m_Canvas.ApplyScale(node->m_OutlineThickness);

		ImU32 fillColor = node->Data->bEnabled ? Node::FillColor : Node::DisabledFillColor;
		drawList->AddCircleFilled(position, radius, fillColor);
		drawList->AddCircle(position, radius, displayedOutlineColor, 0, outlineThickness);

		ImGui::SetWindowFontScale(m_Canvas.ApplyScale(0.8f));
		ImGui::PushFont(nodeFont);
		drawList->AddText(textPos, Node::TextColor, text.c_str());
		ImGui::PopFont();
		ImGui::SetWindowFontScale(1.f);
	}

	void GraphEditor::DrawEdge(const UniquePtr<Edge>& edge)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		ImGuiIO& io = ImGui::GetIO();
		ImFont* nodeFont = ImGui::GetIO().Fonts->Fonts[1];
		CanvasView view = m_Canvas.GetView();

		glm::vec2 firstPoint = ToGlmVec2(edge->Node1->Position);
		glm::vec2 secondPoint = ToGlmVec2(edge->Node2->Position);

		glm::vec2 direction = glm::normalize(secondPoint - firstPoint);
		glm::vec2 startPoint = firstPoint + direction * (edge->Node1->Radius + 2);
		glm::vec2 endPoint = secondPoint - direction * (edge->Node2->Radius + 2);

		ImVec2 firstPos = m_Canvas.FromLocal(ToImVec2(startPoint));
		ImVec2 secondPos = m_Canvas.FromLocal(ToImVec2(endPoint));
		float thickness = m_Canvas.ApplyScale(edge->Thickness);

		ImU32 edgeColor = EdgeColors[edge->Data->Type];
		if (!edge->Data->Node1->bEnabled || !edge->Data->Node2->bEnabled || !edge->Data->bEnabled)
		{
			edgeColor = Edge::DisabledColor;
		}

		drawList->AddLine(firstPos, secondPos, edgeColor, thickness);

		std::string text = std::to_string(edge->Data->Weight);
		glm::vec2 edgeCenter = startPoint + (endPoint - startPoint) / 2.f;
		glm::vec2 rightDirection = glm::vec2{ -direction.y, direction.x };
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		textSize.x *= 0.5f;
		ImVec2 textPos = m_Canvas.FromLocal(ToImVec2(edgeCenter - rightDirection * m_EdgeNumberOffset) - textSize);

		ImGui::SetWindowFontScale(m_Canvas.ApplyScale(0.5f));
		ImGui::PushFont(nodeFont);
		drawList->AddText(textPos, Edge::TextColor, text.c_str());
		ImGui::PopFont();
		ImGui::SetWindowFontScale(1.f);

	}

	void GraphEditor::ArrangeNodes()
	{
		srand(static_cast<unsigned int>(time(nullptr))); // Seed with current time

		const float k = 120.0f;       // Ideal distance between nodes
		const float damping = 0.8f; // Damping factor to reduce oscillations
		const float maxForce = 150.0f; // Maximum allowable force per iteration
		const int maxIterations = 1000; // Maximum number of iterations

		std::vector<ImVec2> initialPositions;
		for (int32 i = 0; i < m_Topology.NodesInNetwork.size(); i++)
		{
			initialPositions.emplace_back(i * 500, i % 2 * 500);
		}

		// Step 1: Initialize positions randomly
		int32 counter = 0;
		int32 networkCouneter = 0;
		for (auto& node : m_Nodes)
		{
			if (counter >= m_Topology.NodesInNetwork[networkCouneter])
			{
				counter = 0;
				networkCouneter++;
			}
			node->Position = ImVec2(rand() % 300 + initialPositions[networkCouneter].x, rand() % 300 + initialPositions[networkCouneter].y); // Random positions within a 1500x1500 space
			counter++;
		}


		// Step 2: Force-directed layout
		for (int iteration = 0; iteration < maxIterations; ++iteration)
		{
			// Reset forces for all nodes
			std::vector<ImVec2> forces(m_Nodes.size(), ImVec2(0.f, 0.f));

			// Calculate repulsive forces between all pairs of nodes
			for (size_t i = 0; i < m_Nodes.size(); ++i)
			{
				for (size_t j = i + 1; j < m_Nodes.size(); ++j)
				{
					ImVec2 delta = m_Nodes[i]->Position - m_Nodes[j]->Position;
					float distance = std::max(1e-4f, sqrt(delta.x * delta.x + delta.y * delta.y)); // Avoid division by zero
					ImVec2 repulsion = ImVec2(delta.x / distance, delta.y / distance) * (k * k / distance);

					forces[i].x += repulsion.x;
					forces[i].y += repulsion.y;
					forces[j].x -= repulsion.x;
					forces[j].y -= repulsion.y;
				}
			}

			// Calculate attractive forces for all edges
			for (const auto& edge : m_Edges)
			{
				Node* node1 = edge->Node1;
				Node* node2 = edge->Node2;

				ImVec2 delta = node1->Position - node2->Position;
				float distance = std::max(1e-4f, sqrt(delta.x * delta.x + delta.y * delta.y));
				ImVec2 attraction = ImVec2(delta.x / distance, delta.y / distance) * (distance * distance / k);

				auto node1Index = std::distance(m_Nodes.begin(), std::find_if(m_Nodes.begin(), m_Nodes.end(),
																			  [&node1](const std::unique_ptr<Node>& n) { return n.get() == node1; }));
				auto node2Index = std::distance(m_Nodes.begin(), std::find_if(m_Nodes.begin(), m_Nodes.end(),
																			  [&node2](const std::unique_ptr<Node>& n) { return n.get() == node2; }));

				forces[node1Index].x -= attraction.x;
				forces[node1Index].y -= attraction.y;
				forces[node2Index].x += attraction.x;
				forces[node2Index].y += attraction.y;
			}

			// Clamp and apply forces to update positions
			for (size_t i = 0; i < m_Nodes.size(); ++i)
			{
				// Clamp force to avoid instability
				float forceMagnitude = sqrt(forces[i].x * forces[i].x + forces[i].y * forces[i].y);
				if (forceMagnitude > maxForce)
				{
					forces[i].x = forces[i].x / forceMagnitude * maxForce;
					forces[i].y = forces[i].y / forceMagnitude * maxForce;
				}

				// Update position with damping
				m_Nodes[i]->Position.x += damping * forces[i].x;
				m_Nodes[i]->Position.y += damping * forces[i].y;
			}
		}
	}




	bool GraphEditor::AreInSameRegionalNetwork(const UniquePtr<Node>& node1, const UniquePtr<Node>& node2)
	{
		return AreInSameRegionalNetwork(node1.get(), node2.get());
	}

	bool GraphEditor::AreInSameRegionalNetwork(const Node* node1, const Node* node2)
	{
		int32 nodesInTotal = 0;
		for (int32 i = 0; i < m_Topology.NodesInNetwork.size(); i++)
		{
			bool node1Here = nodesInTotal <= node1->Data->ID && nodesInTotal + m_Topology.NodesInNetwork[i] > node1->Data->ID;
			bool node2Here = nodesInTotal <= node2->Data->ID && nodesInTotal + m_Topology.NodesInNetwork[i] > node2->Data->ID;
			if (node1Here && node2Here)
				return true;
			else if (node1Here || node2Here)
				return false;
		}
	}

}