#include "IBlu.h"
#include "BluManager.h"
#include "Interfaces/IPluginManager.h"

class FBlu : public IBlu
{

	/** IModuleInterface implementation */
	virtual void StartupModule() override
	{
#if ENGINE_MAJOR_VERSION >= 5
		return;
#endif

		CefString GameDirCef = *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "BluCache");
		FString ExecutablePath = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("BLUI"))->GetBaseDir() + "/ThirdParty/cef/");
		CefString CefLogPath = *FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "Saved/Logs/blui_cef.log");

		// Setup the default settings for BluManager
		BluManager::Settings.windowless_rendering_enabled = true;
		BluManager::Settings.no_sandbox = true;
		BluManager::Settings.uncaught_exception_stack_size = 5;
		BluManager::Settings.multi_threaded_message_loop = false;
		BluManager::Settings.log_severity = LOGSEVERITY_ERROR;

	#if PLATFORM_LINUX
		ExecutablePath = "./blu_ue4_process";
	#endif
	#if PLATFORM_MAC
		ExecutablePath += "Mac/shipping/blu_ue4_process.app/Contents/MacOS/blu_ue4_process";
	#endif
	#if PLATFORM_WINDOWS
		ExecutablePath += "Win/shipping/blu_ue4_process.exe";
	#endif

		CefString realExePath = *ExecutablePath;

		// Set the sub-process path
		CefString(&BluManager::Settings.browser_subprocess_path).FromString(realExePath);
		// Set the cache path
		CefString(&BluManager::Settings.cache_path).FromString(GameDirCef);
		// Cef logs
		CefString(&BluManager::Settings.log_file).FromString(CefLogPath);

		// Make a new manager instance
		CefRefPtr<BluManager> BluApp = new BluManager();

		//CefExecuteProcess(BluManager::main_args, BluApp, NULL);
		CefInitialize(BluManager::MainArgs, BluManager::Settings, BluApp, NULL);

		UE_LOG(LogBlu, Log, TEXT(" STATUS: Loaded"));
	}

	virtual void ShutdownModule() override
	{
		UE_LOG(LogBlu, Log, TEXT(" STATUS: Shutdown"));
		CefShutdown();
	}

};




IMPLEMENT_MODULE( FBlu, Blu )
DEFINE_LOG_CATEGORY(LogBlu);