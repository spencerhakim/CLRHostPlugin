/**
* John Bradley (jrb@turrettech.com)
*/
#pragma once

#include <vector>

#include "CLRHostApi.h"
#include "CLRHost.h"

class CLRHostPlugin
{

public:
    static HINSTANCE hinstDLL;
    static CLRHostPlugin *instance;

private:
    bool isDynamicLocale;
    CLRHostApi *clrApi;
    CLRHost *clrHost;

    std::vector<CLRPlugin *> clrPlugins;

public:
    CLRHostPlugin();
    ~CLRHostPlugin();

public:
    CLRHost *GetCLRHost() { return clrHost; }
    CLRHostApi *GetCLRApi() { return clrApi; }

public:
    void LoadPlugins();
    void UnloadPlugins();

    void OnStartStream();
    void OnStopStream();

    void OnStartStreaming();
    void OnStopStreaming();

    void OnStartRecording();
    void OnStopRecording();

    void OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting);
    void OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps);

    void OnSceneSwitch(CTSTR scene);
    void OnScenesChanged();

    void OnSourceOrderChanged();
    void OnSourceChanged(CTSTR sourceName, XElement* source);
    void OnSourcesAddedOrRemoved();

    void OnMicVolumeChanged(float level, bool muted, bool finalValue);
    void OnDesktopVolumeChanged(float level, bool muted, bool finalValue);

    void OnLogUpdate(CTSTR delta, UINT length);
};
