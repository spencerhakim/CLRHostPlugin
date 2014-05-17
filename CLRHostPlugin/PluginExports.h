#pragma once

#include "OBSApi.h"

#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

EXTERN_DLL_EXPORT CTSTR GetPluginName();
EXTERN_DLL_EXPORT CTSTR GetPluginDescription();

EXTERN_DLL_EXPORT bool LoadPlugin();
EXTERN_DLL_EXPORT void UnloadPlugin();

EXTERN_DLL_EXPORT void OnStartStream();
EXTERN_DLL_EXPORT void OnStopStream();

EXTERN_DLL_EXPORT void OnStartStreaming();
EXTERN_DLL_EXPORT void OnStopStreaming();

EXTERN_DLL_EXPORT void OnStartRecording();
EXTERN_DLL_EXPORT void OnStopRecording();

EXTERN_DLL_EXPORT void OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting);
EXTERN_DLL_EXPORT void OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps);

EXTERN_DLL_EXPORT void OnSceneSwitch(CTSTR scene);
EXTERN_DLL_EXPORT void OnScenesChanged();

EXTERN_DLL_EXPORT void OnSourceOrderChanged();
EXTERN_DLL_EXPORT void OnSourceChanged(CTSTR sourceName, XElement* source);
EXTERN_DLL_EXPORT void OnSourcesAddedOrRemoved();

EXTERN_DLL_EXPORT void OnMicVolumeChanged(float level, bool muted, bool finalValue);
EXTERN_DLL_EXPORT void OnDesktopVolumeChanged(float level, bool muted, bool finalValue);

EXTERN_DLL_EXPORT void OnLogUpdate(CTSTR delta, UINT length);