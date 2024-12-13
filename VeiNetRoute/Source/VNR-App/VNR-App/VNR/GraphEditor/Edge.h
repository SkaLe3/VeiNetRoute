#pragma once
#include <imgui.h>
#include <cstdint>

namespace VNR
{
	struct Node;

	struct Edge
	{
		Node* Node1;
		Node* Node2;

		int32_t Number = 0;
		ImU32 Color = ColorTypes[0];
		ImU32 TextColor = IM_COL32(255, 255, 255, 200);

		inline static ImU32 ColorTypes[3] = {IM_COL32(255, 255, 255, 200),
								IM_COL32(230, 230, 40, 200),
								IM_COL32(230, 40, 40, 200)};

		float Thickness = 4.f;
	};
}