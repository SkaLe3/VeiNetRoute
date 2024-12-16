#pragma once
#include <imgui.h>
#include <cstdint>

namespace VNR
{
	struct NetworkNode;

	struct Node
	{
		NetworkNode* Data;

		ImVec2 Position;
		float Radius;
		bool bSelected = false;
		bool bDragging = false;
		ImVec2 DragOffset = {0.f, 0.f};


		ImU32 FillColor = IM_COL32(59, 109, 140, 255);
		ImU32 OutlineColor = IM_COL32(106, 154, 176, 255); 
		ImU32 SelectedColor = IM_COL32(210, 210, 210, 255); 
		ImU32 TextColor = IM_COL32(255, 255, 255, 240);

		float m_OutlineThickness = 4.0f;
		float m_OutlineThicknessSelected = 8.f;

		Node(NetworkNode* data) : Position({0.f, 0.f}), Radius(30.f), Data(data) {}
	};
}