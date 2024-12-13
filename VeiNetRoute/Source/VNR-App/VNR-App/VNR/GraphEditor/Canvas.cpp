#include "Canvas.h"
#include <algorithm>


namespace VNR
{

	void Canvas::Begin()
	{
		m_WidgetPosition = ImGui::GetCursorScreenPos();
		m_WidgetSize = ImGui::GetContentRegionAvail();
		m_WidgetRect = ImRect(m_WidgetPosition, m_WidgetPosition + m_WidgetSize);

		UpdateViewTransformPosition();
		EnterLocalSpace();		
	}

	void Canvas::End()
	{
		LeaveLocalSpace();
	}

	CanvasView Canvas::GetView() const
	{
		return m_View;
	}

	ImVec2 Canvas::GetPos() const
	{
		 return m_WidgetPosition;
	}

	void Canvas::SetView(const ImVec2& origin, float scale)
	{
		SetView(CanvasView(origin, scale));
	}

	void Canvas::SetView(const CanvasView& view)
	{
		m_View.Origin = view.Origin;
		UpdateViewTransformPosition();
		m_View.Scale = view.Scale;
		m_View.InvScale = view.InvScale;
	}



	CanvasView Canvas::CalcCenterView(const ImVec2& canvasPoint) const
	{
		auto localCenter = ToLocal(m_WidgetPosition + m_WidgetSize * 0.5f);
		auto localOffset = canvasPoint - localCenter;
		auto offset = localOffset * m_View.Scale;
		return CanvasView{ m_View.Origin - offset, m_View.Scale };
	}

	float Canvas::ApplyScale(float value) const
	{
		return value * m_View.Scale;
	}

	void Canvas::SaveInputState()
	{
		auto& io = ImGui::GetIO();
		m_MousePosBackup = io.MousePos;
		m_MousePosPrevBackup = io.MousePosPrev;
		for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); i++)
			m_MouseClickedPosBackup[i] = io.MouseClickedPos[i];

	}

	void Canvas::RestoreInputState()
	{
		auto& io = ImGui::GetIO();
		io.MousePos = m_MousePosBackup;
		io.MousePosPrev = m_MousePosPrevBackup;
		for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); i++)
			io.MouseClickedPos[i] = m_MouseClickedPosBackup[i];
	}

	ImVec2 Canvas::FromLocal(const ImVec2& point) const
	{
		return point * m_View.Scale + m_ViewTransformPosition;
	}

	ImVec2 Canvas::ToLocal(const ImVec2& point) const
	{
		return (point - m_ViewTransformPosition) * m_View.InvScale;
	}

	ImRect Canvas::CalcViewRect(const CanvasView& view) const
	{
		ImRect result;
		result.Min = ImVec2(-view.Origin.x, -view.Origin.y) * view.InvScale;
		result.Max = (m_WidgetSize - view.Origin) * view.InvScale;
		return result;
	}

	void Canvas::EnterLocalSpace()
	{
		SaveInputState();
		auto& io = ImGui::GetIO();
		io.MousePos = (m_MousePosBackup - m_ViewTransformPosition) * m_View.InvScale;
		io.MousePosPrev = (m_MousePosPrevBackup - m_ViewTransformPosition) * m_View.InvScale;
		for (auto i = 0; i < IM_ARRAYSIZE(m_MouseClickedPosBackup); i++)
			io.MouseClickedPos[i] = (m_MouseClickedPosBackup[i] - m_ViewTransformPosition) * m_View.InvScale;

		m_ViewRect = CalcViewRect(m_View);
	}

	void Canvas::LeaveLocalSpace()
	{
		RestoreInputState();
	}

	void Canvas::UpdateViewTransformPosition()
	{
		m_ViewTransformPosition = m_View.Origin + m_WidgetPosition;
	}

}

