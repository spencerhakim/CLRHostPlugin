#pragma once

#include <vector>
#include <metahost.h>

#include "CLRHostApi.h"
#include "CLRPlugin.h"

struct ICLRMetaHost;
struct ICLRRuntimeInfo;
struct ICLRRuntimeHost;
struct ICorRuntimeHost;

namespace mscorlib {
    struct _AppDomain;
    struct _Assembly;
    struct _Type;
}

#define INTEROP_PATH            TEXT("plugins\\CLRHostPlugin\\")
#define INTEROP_PATH_SEARCH     TEXT("plugins\\CLRHostPlugin\\*.dll")
#define INTEROP_ASSEMBLY        TEXT("CLRHost.Interop.dll")
#define INTEROP_ASSEMBLY_PATH   INTEROP_PATH INTEROP_ASSEMBLY
#define INTEROP_LOAD_CLASS      TEXT("CLROBS.API")

class CLRHost {

private:
    CLRHostApi *clrApi;

    ICLRMetaHost *clrMetaHost;
    ICLRRuntimeInfo *clrRuntimeInfo;
    ICorRuntimeHost* corRuntimeHost;

    mscorlib::_AppDomain *appDomain;
    mscorlib::_Assembly *libraryAssembly;


    mscorlib::_Type *libraryType;
    mscorlib::_Type *pluginType;
    mscorlib::_Type *imageSourceType;
    mscorlib::_Type *imageSourceFactoryType;
    mscorlib::_Type *settingsPaneType;
    mscorlib::_Type *xElementType;

    IUnknown *libraryInstance;

    TCHAR *clrRuntimeVersion;

    bool isInitialized;
    bool isLibraryLoaded;

    std::vector<CLRPlugin *> clrPlugins;

public:
    CLRHost::CLRHost(TCHAR *clrRuntimeVersion, CLRHostApi *clrApi);
    ~CLRHost();

public:
    bool Initialize();
    bool LoadInteropLibrary();

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

public:
    mscorlib::_Type *GetImageSourceType() { return imageSourceType; }
    mscorlib::_Type *GetImageSourceFactoryType() { return imageSourceFactoryType; }
    mscorlib::_Type *GetSettingsPaneType() { return settingsPaneType; }
    mscorlib::_Type *GetXElementType() { return xElementType; }

};