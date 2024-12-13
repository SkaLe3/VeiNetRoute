#include <VNR-Core/Core/EntryPoint.h>

#include "EditorLayer.h"
#include "VNR/VNRLayer.h"

namespace VNR
{
	class VeiMEditor : public Application
	{
	public:
		VeiMEditor(const ApplicationSpecification& appSpecs)
			: Application(appSpecs)
		{

		}

		~VeiMEditor()
		{

		}
	};

	Application* CreateApplication(int argc, char** argv)
	{
		ApplicationSpecification specs;

		specs.AppConfig.Name = "VeiNetRoute Simulator";
		specs.AppConfig.EnableConsole = true; // TODO: Add option to enable console in game shipping build, or to choose it instead of editor console in debug and development editor build
		specs.AppConfig.WorkingDirectory = std::filesystem::current_path().string() +  "/../../";
		specs.AppConfig.EngineContentPath = "Content/";

		specs.WndConfig.Title = "VeiNetRoute Simulator";
		specs.WndConfig.Width = 1600;
		specs.WndConfig.Height = 900;
		specs.WndConfig.VSync = true;
		specs.WndConfig.CustomTitlebar = true;
		specs.WndConfig.WindowResizeable = true;
		specs.WndConfig.IconPath = "UI/Icons/VeiM_Logo.png";
		specs.WndConfig.Mode = EWindowMode::Windowed;


		Application* app = new VeiMEditor(specs);
		app->PushLayer(new EditorLayer());
		app->PushLayer(new VNRLayer());
		return app;
	}
}


