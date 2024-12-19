#pragma once
#include "VNR-App/Utils/Math/MathHelpers.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>




namespace VNR
{


	struct CanvasView
	{
		ImVec2 Origin;
		float Scale = 1.0f;
		float InvScale = 1.0f;

		CanvasView() = default;
		CanvasView(const ImVec2& origin, float scale) : Origin(origin), Scale(scale), InvScale(scale ? 1.0f / scale : 0.0f) {}

		void Set(const ImVec2& origin, float scale)
		{
			*this = CanvasView(origin, scale);
		}

	};

	class Canvas
	{
	public:
		void Begin();
		void End();

		CanvasView GetView() const;
		ImVec2 GetPos() const;
		void SetView(const ImVec2& origin, float scale);
		void SetView(const CanvasView& view);
		CanvasView CalcCenterView(const ImVec2& canvasPoint) const;
		float ApplyScale(float value) const;

		void SaveInputState();
		void RestoreInputState();
		ImVec2 FromLocal(const ImVec2& point) const;
		ImVec2 ToLocal(const ImVec2& point) const;
		ImRect CalcViewRect(const CanvasView& view) const;
		void EnterLocalSpace();
		void LeaveLocalSpace();
		void UpdateViewTransformPosition();


	private:
		ImVec2 m_WidgetPosition;
		ImVec2 m_WidgetSize;
		ImRect m_WidgetRect;

		CanvasView m_View;
		ImRect m_ViewRect;
		ImVec2 m_ViewTransformPosition;

		ImVec2 m_MousePosBackup;
		ImVec2 m_MousePosPrevBackup;
		ImVec2 m_MouseClickedPosBackup[IM_ARRAYSIZE(ImGuiIO::MouseClickedPos)];
		ImVec2 m_WindowCUrsorMaxBackup;


	};

}