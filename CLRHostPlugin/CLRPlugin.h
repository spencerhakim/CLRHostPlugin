#pragma once

#include <map>
#include <comdef.h>
#include "CLRObject.h"

namespace mscorlib {
    struct _EventInfo;
    struct _MethodInfo;
}

class CLRPlugin : public CLRObject
{
protected:
    std::map<std::string, mscorlib::_MethodInfo*> methodInfos;
    void Invoke(std::string funcMacro, LPSAFEARRAY parameters);

public:
    virtual bool Attach(CLRObjectRef &clrObjectRef, mscorlib::_Type *objectType);
    virtual void Detach();

public:
    std::wstring GetPluginName();
    std::wstring GetPluginDescription();

    bool LoadPlugin();
    void UnloadPlugin();

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