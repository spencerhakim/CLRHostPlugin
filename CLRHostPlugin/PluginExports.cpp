#include "Stdafx.h"
#include "PluginExports.h"

#define SAFE_HOST_PLUGIN_CALL(x) if(CLRHostPlugin::instance == NULL)return; CLRHostPlugin::instance->x

BOOL CALLBACK DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        CLRHostPlugin::hinstDLL = hinstDLL;
    }
    return TRUE;
}

CTSTR GetPluginName()
{
    return STR("PluginName");
}

CTSTR GetPluginDescription()
{
    return STR("PluginDescription");
}

bool LoadPlugin()
{
    if (CLRHostPlugin::instance != NULL) {
        return false;
    }
    CLRHostPlugin::instance = new CLRHostPlugin();
    CLRHostPlugin::instance->LoadPlugins();
    return true;
}

void UnloadPlugin()
{
    if (CLRHostPlugin::instance == NULL) {
        return;
    }

    CLRHostPlugin::instance->UnloadPlugins();
    delete CLRHostPlugin::instance;
    CLRHostPlugin::instance = NULL;
}

void OnStartStream()
{
    SAFE_HOST_PLUGIN_CALL(OnStartStream());
}

void OnStopStream()
{
    SAFE_HOST_PLUGIN_CALL(OnStopStream());
}

void OnStartStreaming()
{
    SAFE_HOST_PLUGIN_CALL(OnStartStreaming());
}

void OnStopStreaming()
{
    SAFE_HOST_PLUGIN_CALL(OnStopStreaming());
}

void OnStartRecording()
{
    SAFE_HOST_PLUGIN_CALL(OnStartRecording());
}

void OnStopRecording()
{
    SAFE_HOST_PLUGIN_CALL(OnStopRecording());
}

void OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting)
{
    SAFE_HOST_PLUGIN_CALL(OnOBSStatus(running, streaming, recording, previewing, reconnecting));
}

void OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps)
{
    SAFE_HOST_PLUGIN_CALL(OnStreamStatus(streaming, previewOnly, bytesPerSec, strain, totalStreamTime, totalNumFrames, numDroppedFrames, fps));
}

void OnSceneSwitch(CTSTR scene)
{
    SAFE_HOST_PLUGIN_CALL(OnSceneSwitch(scene));
}

void OnScenesChanged()
{
    SAFE_HOST_PLUGIN_CALL(OnScenesChanged());
}

void OnSourceOrderChanged()
{
    SAFE_HOST_PLUGIN_CALL(OnSourceOrderChanged());
}

void OnSourceChanged(CTSTR sourceName, XElement* source)
{
    SAFE_HOST_PLUGIN_CALL(OnSourceChanged(sourceName, source));
}

void OnSourcesAddedOrRemoved()
{
    SAFE_HOST_PLUGIN_CALL(OnSourcesAddedOrRemoved());
}

void OnMicVolumeChanged(float level, bool muted, bool finalValue)
{
    SAFE_HOST_PLUGIN_CALL(OnMicVolumeChanged(level, muted, finalValue));
}

void OnDesktopVolumeChanged(float level, bool muted, bool finalValue)
{
    SAFE_HOST_PLUGIN_CALL(OnDesktopVolumeChanged(level, muted, finalValue));
}

void OnLogUpdate(CTSTR delta, UINT length)
{
    SAFE_HOST_PLUGIN_CALL(OnLogUpdate(delta, length));
}
