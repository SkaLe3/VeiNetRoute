#include "GraphEditor.h"
#include "VNR-App/VNR/Network/NetworkNode.h"
#include "VNR-App/VNR/Network/Channel.h"

#include <string>
#include <iostream>

namespace VNR
{

	GraphEditor::GraphEditor()
	{

	}

	void GraphEditor::GenerateGraph(std::vector<UniquePtr<NetworkNode>>& nodes, std::vector<UniquePtr<Channel>>& channels)
	{
		m_Nodes.clear();
		m_Edges.clear();
		int i = 0;
		for (const auto& node : nodes)
		{
			m_Nodes.push_back(MakeUnique<Node>(node.get()));
			node->Visuals = m_Nodes.back().get();
			m_Nodes.back()->Position = {float(i * 100), float(i % 3 * 300)};
			i++;

		}
		for (const auto& channel : channels)
		{
			m_Edges.push_back(MakeUnique<Edge>(channel.get(), channel->Node1->Visuals, channel->Node2->Visuals));
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
		for (const auto& node : m_Nodes)
		{
			DrawNode(node);
		}
		for (const auto& edge : m_Edges)
		{
			DrawEdge(edge);
		}

		m_Canvas.End();
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

			for (auto& node : m_Nodes)
			{
				if (ImLengthSqr(io.MousePos - node->Position) <= (node->Radius * node->Radius))
				{
					node->bSelected = true;
					anyNodeSelected = true;
				}
				else
				{
					node->bSelected = false;
				}
			}
		}
	}

	void GraphEditor::DragNode()
	{
		ImGuiIO& io = ImGui::GetIO();
		for (auto& node : m_Nodes)
		{
			if (ImGui::IsMouseClicked(0))
			{
				if (ImLengthSqr(io.MousePos - node->Position) <= (node->Radius * node->Radius))
				{
					node->bDragging = true;
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

		ImU32 outlineColor = node->bSelected ? node->SelectedColor : node->OutlineColor;
		float outlineThickness = node->bSelected ?
			m_Canvas.ApplyScale(node->m_OutlineThicknessSelected) : m_Canvas.ApplyScale(node->m_OutlineThickness);

		drawList->AddCircleFilled(position, radius, node->FillColor);
		drawList->AddCircle(position, radius, outlineColor, 0, outlineThickness);

		ImGui::SetWindowFontScale(m_Canvas.ApplyScale(0.8f));
		ImGui::PushFont(nodeFont);
		drawList->AddText(textPos, node->TextColor, text.c_str());
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

		drawList->AddLine(firstPos, secondPos, edge->Color, thickness);

		std::string text = std::to_string(edge->Data->Weight);
		glm::vec2 edgeCenter = startPoint + (endPoint - startPoint) / 2.f;
		glm::vec2 rightDirection = glm::vec2{ -direction.y, direction.x };
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		textSize.x *= 0.5f;
		ImVec2 textPos = m_Canvas.FromLocal(ToImVec2(edgeCenter - rightDirection * m_EdgeNumberOffset) - textSize);

		ImGui::SetWindowFontScale(m_Canvas.ApplyScale(0.5f));
		ImGui::PushFont(nodeFont);
		drawList->AddText(textPos, edge->TextColor, text.c_str());
		ImGui::PopFont();
		ImGui::SetWindowFontScale(1.f);

	}

}