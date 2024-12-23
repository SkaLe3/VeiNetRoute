#pragma once
#include "VNR-Core/Core/Layer.h"
#include "Titlebar.h"
#include "EditorWindows.h"

namespace VNR
{
	class EditorLayer : public Layer
	{
	public: 
		EditorLayer();
		void OnAttach() override;
		void OnGUI() override;

		void ImGuiWindowMenu();
		void ImGuiWindowsRender();

	public:
		void ThemeEditorRender();


	private:
		std::shared_ptr<UI::TitleBar> m_TitleBar;
		float m_WindowBorderSize = 1.0f;
		float m_MinWinSizeX = 100.0f;
		bool show_demo_window = false;
		bool show_another_window = false;
		bool m_ThemeEditor = false;
		ImGuiWindows m_ImGuiWindows;
	};
}