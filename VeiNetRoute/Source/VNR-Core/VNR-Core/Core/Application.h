#pragma once


#include "VNR-Core/ImGui/GUIContext.h"
#include "Image.h"


#include "VNR-Core/Core/Window.h"
#include "LayerStack.h"

#include <filesystem>
#include <string>
#include <functional>
#include <memory>

struct GLFWwindow;

namespace VNR
{

	struct ApplicationConfig
	{
		std::string Name = "VeiNetRoute";
		bool EnableConsole = true;
		std::filesystem::path WorkingDirectory;
		std::filesystem::path EngineContentPath;
		// TODO: Add more
	};


	struct ApplicationSpecification		  
	{
		ApplicationConfig AppConfig;
		WindowConfig WndConfig;
	};

	class Application
	{
	public:
		Application(const ApplicationSpecification& applicationSpecification);
		~Application();
		static Application& Get();

		void Run();


		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close() { m_Running = false; }

		inline std::filesystem::path GetEngineResourcePath() { return m_Config.EngineContentPath; }
		inline float GetDeltaTime() { return m_DeltaTime; }
		Window& GetWindow() const { return *m_Window; }

		VNR::GUIContext* GetGUIContext() { return m_GUIContext; }
	private: 
		void InitGUI();
		void RenderGUI();

	private:
		void Shutdown();

		void OnEvent(const std::string& inf);
	private:

		VNR::GUIContext* m_GUIContext;

	private:
		ApplicationConfig m_Config;
		std::unique_ptr<Window> m_Window;

		LayerStack m_LayerStack;
		
		bool m_Running = true;
		bool m_Minimized = false;

		float m_DeltaTime = 0.0f;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	Application* CreateApplication(int argc, char** argv);
}

