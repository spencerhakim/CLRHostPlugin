/**
* John Bradley (jrb@turrettech.com)
*/
#include "Stdafx.h"
#include "CLRHostPlugin.h"

HINSTANCE CLRHostPlugin::hinstDLL = NULL;
CLRHostPlugin *CLRHostPlugin::instance = NULL;

CLRHostPlugin::CLRHostPlugin() {

    CLRHostPlugin::instance = this;

    isDynamicLocale = false;

    if (!locale->HasLookup(KEY("PluginName"))) {
        isDynamicLocale = true;
        int localizationStringCount = sizeof(localizationStrings) / sizeof(CTSTR);
        Log(TEXT("CLR host plugin strings not found, dynamically loading %d strings"), sizeof(localizationStrings) / sizeof(CTSTR));
        for(int i = 0; i < localizationStringCount; i += 2) {
            locale->AddLookupString(localizationStrings[i], localizationStrings[i+1]);
        }
        if (!locale->HasLookup(KEY("PluginName"))) {
            AppWarning(TEXT("Uh oh..., unable to dynamically add our localization keys"));
        }
    }

    clrApi = new CLRHostApi();
    clrHost = new CLRHost(nullptr, clrApi);
    clrHost->Initialize();     
}

CLRHostPlugin::~CLRHostPlugin() {

    if (isDynamicLocale) {
        int localizationStringCount = sizeof(localizationStrings) / sizeof(CTSTR);
        Log(TEXT("CLR host plugin instance deleted; removing dynamically loaded localization strings"));
        for(int i = 0; i < localizationStringCount; i += 2) {
            locale->RemoveLookupString(localizationStrings[i]);
        }
    }

    isDynamicLocale = false;

    if (clrHost) {
        delete clrHost;
        clrHost = nullptr;
    }

    if (clrApi) {
        delete clrApi;
        clrApi = nullptr;
    }
}

void CLRHostPlugin::LoadPlugins()
{

    if (clrHost->LoadInteropLibrary()) {
        clrHost->LoadPlugins();
    }
}

void CLRHostPlugin::UnloadPlugins()
{
    clrHost->UnloadPlugins();
}

void CLRHostPlugin::OnStartStream()
{ 
    clrHost->OnStartStream();
}

void CLRHostPlugin::OnStopStream()
{
    clrHost->OnStopStream();
}

void CLRHostPlugin::OnStartStreaming()
{
    clrHost->OnStartStreaming();
}

void CLRHostPlugin::OnStopStreaming()
{
    clrHost->OnStopStreaming();
}

void CLRHostPlugin::OnStartRecording()
{
    clrHost->OnStartRecording();
}

void CLRHostPlugin::OnStopRecording()
{
    clrHost->OnStopRecording();
}

void CLRHostPlugin::OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting)
{
    clrHost->OnOBSStatus(running, streaming, recording, previewing, reconnecting);
}

void CLRHostPlugin::OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps)
{
    clrHost->OnStreamStatus(streaming, previewOnly, bytesPerSec, strain, totalStreamTime, totalNumFrames, numDroppedFrames, fps);
}

void CLRHostPlugin::OnSceneSwitch(CTSTR scene)
{
    clrHost->OnSceneSwitch(scene);
}

void CLRHostPlugin::OnScenesChanged()
{
    clrHost->OnScenesChanged();
}

void CLRHostPlugin::OnSourceOrderChanged()
{
    clrHost->OnSourceOrderChanged();
}

void CLRHostPlugin::OnSourceChanged(CTSTR sourceName, XElement* source)
{
    clrHost->OnSourceChanged(sourceName, source);
}

void CLRHostPlugin::OnSourcesAddedOrRemoved()
{
    clrHost->OnSourcesAddedOrRemoved();
}

void CLRHostPlugin::OnMicVolumeChanged(float level, bool muted, bool finalValue)
{
    clrHost->OnMicVolumeChanged(level, muted, finalValue);
}

void CLRHostPlugin::OnDesktopVolumeChanged(float level, bool muted, bool finalValue)
{
    clrHost->OnDesktopVolumeChanged(level, muted, finalValue);
}

void CLRHostPlugin::OnLogUpdate(CTSTR delta, UINT length)
{
    clrHost->OnLogUpdate(delta, length);
}
