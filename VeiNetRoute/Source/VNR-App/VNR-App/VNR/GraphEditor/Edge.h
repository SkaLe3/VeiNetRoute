#pragma once
#include "VNR-Core/Core/CoreDefines.h"
#include <imgui.h>
#include <cstdint>

namespace VNR
{
	struct Node;
	struct Channel;

	struct Edge
	{
		Channel* Data;

		Node* Node1;
		Node* Node2;

		inline static constexpr ImU32 TextColor = IM_COL32(255, 255, 255, 200);
		inline static constexpr ImU32 DisabledColor = IM_COL32(200, 200, 200, 200);
		inline static constexpr ImU32 HighlightColor = IM_COL32(30, 230, 35, 255);

		float Thickness = 4.f;

		Edge(Channel* data, Node* node1, Node* node2) : Data(data), Node1(node1), Node2(node2) {}
	};
}