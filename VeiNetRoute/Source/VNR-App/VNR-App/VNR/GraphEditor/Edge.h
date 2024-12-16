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

		ImU32 Color = ColorTypes[0];
		ImU32 TextColor = IM_COL32(255, 255, 255, 200);

		inline static ImU32 ColorTypes[3] = {IM_COL32(255, 255, 255, 200),
								IM_COL32(230, 230, 40, 200),
								IM_COL32(230, 40, 40, 200)};

		float Thickness = 4.f;

		Edge(Channel* data, Node* node1, Node* node2) : Data(data), Node1(node1), Node2(node2) {}
	};
}