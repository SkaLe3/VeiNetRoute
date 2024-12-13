#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>		 


#include "VNR-Core/Platform/Windows/WindowsUtils.h"
#include "VNR-Core/IO/Log.h"

extern bool g_ApplicationRunning;

namespace VeiM
{
	Application* Application::s_Instance = nullptr;
	Application::Application(const ApplicationSpecification& applicationSpecification)
		: m_Config(applicationSpecification.AppConfig)
	{
		VNR_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		if (!m_Config.WorkingDirectory.empty())
			std::filesystem::current_path(m_Config.WorkingDirectory);



		m_Window = std::make_unique<Window>(applicationSpecification.WndConfig);  // TODO: Make static function Create() or another
		m_Window->SetEventCallback([this](const std::string& inf) { Application::OnEvent(inf); });
		InitGUI();
	}

	Application::~Application()
	{
		Shutdown();
		s_Instance = nullptr;
	}

	void Application::InitGUI()
	{
		m_GUIContext = new GUIContext();
		PushOverlay(m_GUIContext);
	}

	void Application::RenderGUI()
	{
		m_GUIContext->BeginFrame();
		m_GUIContext->EnableDocking(*m_Window);
		{
			for (Layer* layer : m_LayerStack)
				layer->OnGUI();
		}


		m_GUIContext->EndFrame();
	}
	void Application::Shutdown()
	{

		g_ApplicationRunning = false;
	}



	Application& Application::Get()
	{
		return *s_Instance;
	}

	void Application::OnEvent(const std::string& inf)
	{
		// TODO: Dispatch events, change string to event type
		VNR_CORE_INFO(inf);
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window->GetNativeWindow()) && m_Running)
		{
			m_Window->PollEvents();

			float time = Time::GetTime();
			m_DeltaTime = time - m_LastFrameTime;
			m_LastFrameTime = time;


			// TODO: Put in RHI
			int display_w, display_h;
			glfwGetFramebufferSize(m_Window->GetNativeWindow(), &display_w, &display_h);
			glViewport(0, 0, display_w, display_h);
			glClearColor(0.8, 0.8, 0.8, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			RenderGUI();

			m_Window->SwapBuffers();
		}
	}



	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushLayer(overlay);
		overlay->OnAttach();
	}

}