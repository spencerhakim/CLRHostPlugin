#include "Stdafx.h"
#include "CLRHost.h"

#pragma comment(lib, "mscoree.lib") 
#import "mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent") 
using namespace mscorlib; 

#define FOREACH_PLUGIN(x) for(auto i = clrPlugins.begin(); i < clrPlugins.end(); i++) { (*i)->x; }

CLRHost::CLRHost(TCHAR *clrRuntimeVersion, CLRHostApi *clrApi) 
{
    clrMetaHost = nullptr;
    clrRuntimeInfo = nullptr;
    corRuntimeHost = nullptr;
    this->clrRuntimeVersion = clrRuntimeVersion;
    isInitialized = false;
    isLibraryLoaded = false;
    this->clrApi = clrApi;
}

CLRHost::~CLRHost()
{
    if (isInitialized) {
        corRuntimeHost->Stop();

        if (clrMetaHost) { 
            clrMetaHost->Release(); 
            clrMetaHost = nullptr; 
        } 
        if (clrRuntimeInfo) { 
            clrRuntimeInfo->Release(); 
            clrRuntimeInfo = nullptr; 
        } 
        if (corRuntimeHost) { 
            corRuntimeHost->Release(); 
            corRuntimeHost = nullptr; 
        }
    }

    if (isLibraryLoaded) {
        if (appDomain) {
            appDomain->Release();
            appDomain = nullptr;
        }
        if (libraryAssembly) {
            libraryAssembly->Release();
            libraryAssembly = nullptr;
        }
        if (libraryType) {
            libraryType->Release();
            libraryType = nullptr;
        }
        if (imageSourceType) {
            imageSourceType->Release();
            imageSourceType = nullptr;
        }
        if (imageSourceFactoryType) {
            imageSourceFactoryType->Release();
            imageSourceFactoryType = nullptr;
        }
        if (settingsPaneType) {
            settingsPaneType->Release();
            settingsPaneType = nullptr;
        }
        if (xElementType) {
            xElementType->Release();
            xElementType = nullptr;
        }
        if (libraryInstance) {
            libraryInstance->Release();
        }
    }
}

HRESULT GetInstalledClrRuntimes(ICLRMetaHost *clrMetaHost, std::vector<std::wstring>& clrRuntimeList)
{
    HRESULT hr = S_OK;		
    clrRuntimeList.clear();

    IEnumUnknown* runtimeEnumerator = nullptr;
    hr = clrMetaHost->EnumerateInstalledRuntimes(&runtimeEnumerator);
    if (SUCCEEDED(hr)) {
        WCHAR currentRuntime[50];
        DWORD bufferSize = ARRAYSIZE(currentRuntime);
        IUnknown* runtime = nullptr;
        while (runtimeEnumerator->Next(1, &runtime, NULL) == S_OK)
        {
            ICLRRuntimeInfo* runtimeInfo = nullptr;
            hr = runtime->QueryInterface(IID_PPV_ARGS(&runtimeInfo));
            if (SUCCEEDED(hr))
            {
                hr = runtimeInfo->GetVersionString(currentRuntime, &bufferSize);
                if (SUCCEEDED(hr))
                {
                    clrRuntimeList.push_back(std::wstring(currentRuntime));
                }
                runtimeInfo->Release();
            }
            runtime->Release();
        }
        runtimeEnumerator->Release();
        hr = S_OK;
    }

    return hr;
}

void GetFilesInDirectory(std::vector<std::wstring> &list, std::wstring pattern, int attributes)
{

    WIN32_FIND_DATA search_data;

    memset(&search_data, 0, sizeof(WIN32_FIND_DATA));

    HANDLE handle = FindFirstFile(pattern.c_str(), &search_data);

    while(handle != INVALID_HANDLE_VALUE)
    {
        if (search_data.dwFileAttributes & attributes) {
            std::wstring file(search_data.cFileName);
            if (file != INTEROP_ASSEMBLY) {
                list.push_back(file);
            }
        }

        if(FindNextFile(handle, &search_data) == FALSE) {
            break;
        }
    }

    FindClose(handle);
}

bool CLRHost::Initialize() 
{
    assert(!isInitialized);

    HRESULT hr; 
    std::vector<std::wstring> clrRuntimeList;

    Log(TEXT("CLRHost::Initialize() attempting to load and start the .NET runtime %s"), clrRuntimeVersion);

    hr = CLRCreateInstance(CLSID_CLRMetaHost, IID_PPV_ARGS(&clrMetaHost)); 
    if (FAILED(hr)) { 
        Log(TEXT("CLRCreateInstance() failed: 0x%08lx"), hr); 
        goto errorCleanup; 
    } 

    if (clrRuntimeVersion) {
        hr = clrMetaHost->GetRuntime(clrRuntimeVersion, IID_PPV_ARGS(&clrRuntimeInfo)); 
        if (FAILED(hr)) { 
            Log(TEXT("ICLRMetaHost::GetRuntime(%s) failed: 0x%08lx"), clrRuntimeVersion, hr); 
        }
    }

    if (!clrRuntimeInfo) {
        hr = GetInstalledClrRuntimes(clrMetaHost, clrRuntimeList);
        if (SUCCEEDED(hr)) {
            if (clrRuntimeList.size()) {
                for(auto itor = clrRuntimeList.begin(); itor < clrRuntimeList.end(); itor++) {
                    Log(TEXT("CLRHost::Initialize() Found version %s .NET runtime"), (*(itor)).c_str());
                }
                std::wstring version = *(clrRuntimeList.end() - 1);
                Log(TEXT("CLRHost::Initialize() attempting to use %s .NET runtime"), version.c_str());

                hr = clrMetaHost->GetRuntime(version.c_str(), IID_PPV_ARGS(&clrRuntimeInfo)); 
                if (FAILED(hr)) { 
                    Log(TEXT("ICLRMetaHost::GetRuntime(%s) failed: 0x%08lx"), version.c_str(), hr); 
                    goto errorCleanup; 
                }
            } else {
                Log(TEXT("CLRHost::Initialize() no .NET Runtimes found!"));
                goto errorCleanup;
            }
        } else {
            Log(TEXT("CLRHost::GetInstalledClrRuntimes) failed: 0x%08lx"), hr); 
            goto errorCleanup;
        }
    }

    BOOL isLoadable; 
    hr = clrRuntimeInfo->IsLoadable(&isLoadable); 
    if (FAILED(hr)) { 
        Log(TEXT("ICLRRuntimeInfo::IsLoadable() failed: 0x%08lx"), hr); 
        goto errorCleanup; 
    } 

    if (!isLoadable) { 
        Log(TEXT(".NET runtime %s cannot be loaded"), clrRuntimeVersion); 
        goto errorCleanup; 
    } 

    hr = clrRuntimeInfo->GetInterface(CLSID_CorRuntimeHost, IID_ICorRuntimeHost, (LPVOID *)&corRuntimeHost);
    if (FAILED(hr)) { 
        Log(TEXT("ICLRRuntimeInfo::GetInterface() failed when retrieving ICorRuntimeHost instance: 0x%08lx"), hr); 
        goto errorCleanup; 
    }

    // Start the CLR. 
    hr = corRuntimeHost->Start(); 
    if (FAILED(hr)) { 
        Log(TEXT("ICorRuntimeHost::Start() failed: 0x%08lx"), hr); 
        goto errorCleanup; 
    }

    isInitialized = true;
    goto success;

errorCleanup: 

    if (clrMetaHost) { 
        clrMetaHost->Release(); 
        clrMetaHost = nullptr; 
    } 
    if (clrRuntimeInfo) { 
        clrRuntimeInfo->Release(); 
        clrRuntimeInfo = nullptr; 
    } 
    if (corRuntimeHost) {
        corRuntimeHost->Release();
        corRuntimeHost = nullptr;
    }

success:
    return isInitialized; 
}

bool CLRHost::LoadInteropLibrary() 
{
    if (!isInitialized) {
        Log(TEXT("CLRHost::LoadInteropLibrary() Runtime not initialized, examine log for cause"));
        return false;
    }

    HRESULT hr;

    IUnknown *appDomainSetupUnknown = NULL;
    IAppDomainSetup *appDomainSetup = NULL;
    IUnknown *appDomainUnknown = NULL;

    bstr_t bstrPluginPath(INTEROP_PATH);
    bstr_t isShadowCopyFiles("true");
    bstr_t interopAssemblyDll(INTEROP_ASSEMBLY);
    bstr_t interopAssemblyName("CLRHost.Interop");

    bstr_t assemblyClassName("CLROBS.API");
    bstr_t pluginClassName("CLROBS.Plugin");
    bstr_t imageSourceTypeName("CLROBS.ImageSource");
    bstr_t imageSourceFactoryTypeName("CLROBS.ImageSourceFactory");
    bstr_t settingsPaneTypeName("CLROBS.SettingsPane");
    bstr_t xElementTypeName("CLROBS.XElement");

    SAFEARRAY *constructorArgs = nullptr;
    SAFEARRAY *apiArgs = nullptr;
    LONG argIndex;
    variant_t clrApiPtr((long long)clrApi);
    variant_t libraryPtr;

    hr = corRuntimeHost->CreateDomainSetup(&appDomainSetupUnknown);
    if (FAILED(hr)) {
        Log(TEXT("ICorRuntimeHost::CreateDomainSetup() failed: 0x%08lx"), hr);
        goto errorCleanup;
    }

    hr = appDomainSetupUnknown->QueryInterface( __uuidof( mscorlib::IAppDomainSetup), (void**)&appDomainSetup);
    if (FAILED(hr)) {
        Log(TEXT("IAppDomainSetup::QueryInterface() failed: 0x%08lx"), hr);
        goto errorCleanup;
    }
    hr = appDomainSetup->put_ApplicationBase(bstrPluginPath);
    if (FAILED(hr)) {
        Log(TEXT("IAppDomainSetup::put_ApplicationBase(%s) failed: 0x%08lx"), INTEROP_PATH, hr);
        goto errorCleanup;
    }
    hr = appDomainSetup->put_ShadowCopyFiles(isShadowCopyFiles);
    if (FAILED(hr)) {
        Log(TEXT("IAppDomainSetup::put_ShadowCopyFiles(%s) failed: 0x%08lx"), TEXT("TRUE"), hr);
        goto errorCleanup;
    }
    hr = appDomainSetup->put_ApplicationName(interopAssemblyDll);
    if (FAILED(hr)) {
        Log(TEXT("IAppDomainSetup::put_ApplicationName(%s) failed: 0x%08lx"), INTEROP_ASSEMBLY, hr);
        goto errorCleanup;
    }
    {
        std::vector<std::wstring> files;
        GetFilesInDirectory(files, INTEROP_PATH TEXT("*"), FILE_ATTRIBUTE_DIRECTORY);
        std::wstring combinedPath;
        for(auto itor = files.begin(); itor < files.end(); itor++)
        {
            if (*itor != TEXT(".") && *itor != TEXT("..")) {
                combinedPath.append(*itor).append(L";");
            }
        }
   
        bstr_t combinedPathString(combinedPath.c_str());
        hr = appDomainSetup->put_PrivateBinPath(combinedPathString);
        if (FAILED(hr)) {
            Log(TEXT("IAppDomainSetup::put_PrivateBinPath(%s) failed: 0x%08lx"), INTEROP_ASSEMBLY, hr);
            goto errorCleanup;
        }
    }

    hr = corRuntimeHost->CreateDomainEx(interopAssemblyDll, appDomainSetup, nullptr, &appDomainUnknown);
    if (FAILED(hr)) {
        Log(TEXT("ICorRuntimeHost::CreateDomainEx(%s, ...) failed: 0x%08lx"), INTEROP_ASSEMBLY, hr);
        goto errorCleanup;
    }

    appDomainSetup->Release();
    appDomainSetup = nullptr;

    hr = appDomainUnknown->QueryInterface(__uuidof( mscorlib::_AppDomain ), (void**)&appDomain);
    if (FAILED(hr)) {
        Log(TEXT("IAppDomain::QueryInterface(%s, ...) failed: 0x%08lx"), hr);
        goto errorCleanup;
    }
    appDomainUnknown->Release();
    appDomainUnknown = nullptr;

    Log(TEXT("CLRHost::LoadInteropLibrary() load the assembly %s"), INTEROP_ASSEMBLY_PATH); 
    hr = appDomain->Load_2(interopAssemblyName, &libraryAssembly);
    if (FAILED(hr)) { 
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to load the assembly: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(assemblyClassName, &libraryType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of interop library API class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(pluginClassName, &pluginType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of Plugin class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(imageSourceTypeName, &imageSourceType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of ImageSource class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(imageSourceFactoryTypeName, &imageSourceFactoryType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of ImageSourceFactory class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(settingsPaneTypeName, &settingsPaneType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of SettingsPane class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = libraryAssembly->GetType_2(xElementTypeName, &xElementType);
    if (FAILED(hr)) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to get type definition of XElement class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    apiArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);
    argIndex = 0;

    hr = SafeArrayPutElement(apiArgs, &argIndex, &clrApiPtr); 
    if (FAILED(hr)) 
    { 
        wprintf(L"SafeArrayPutElement failed: 0x%08lx", hr); 
        goto errorCleanup; 
    }

    hr = libraryAssembly->CreateInstance_3(assemblyClassName, false, BindingFlags_Default, nullptr, apiArgs, nullptr, nullptr, &libraryPtr);
    if (FAILED(hr) || !libraryPtr.punkVal) {
        Log(TEXT("CLRHost::LoadInteropLibrary() failed to instantiate our interop library class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    libraryInstance = libraryPtr.punkVal;
    libraryInstance->AddRef();


    SafeArrayDestroy(apiArgs);
    apiArgs = nullptr;

    isLibraryLoaded = true;
    goto success;

errorCleanup:
    if (appDomainSetup) {
        appDomainSetup->Release();
        appDomainSetup = nullptr;
    }
    if (appDomainUnknown) {
        appDomainUnknown->Release();
        appDomainUnknown = nullptr;
    }
    if (appDomain) {
        appDomain->Release();
        appDomain = nullptr;
    }
    if (libraryAssembly) {
        libraryAssembly->Release();
        libraryAssembly = nullptr;
    }
    if (libraryType) {
        libraryType->Release();
        libraryType = nullptr;
    }
    if (imageSourceType) {
        imageSourceType->Release();
        imageSourceType = nullptr;
    }
    if (imageSourceFactoryType) {
        imageSourceFactoryType->Release();
        imageSourceFactoryType = nullptr;
    }
    if (settingsPaneType) {
        settingsPaneType->Release();
        settingsPaneType = nullptr;
    }
    if (xElementType) {
        xElementType->Release();
        xElementType = nullptr;
    }
    if (apiArgs) {
        SafeArrayDestroy(apiArgs);
    }

    return false;
success:
    return true;
}

CLRPlugin *CreatePluginInstance(std::wstring &typeName, _Type *type, _Type *pluginType, _Type *libraryType, IUnknown *libraryInstance) {
    HRESULT hr;

    _ConstructorInfo *constructor = nullptr;
    SAFEARRAY *constructorArgs = nullptr;

    VARIANT_BOOL isAbstract;
    VARIANT_BOOL isAssignable;

    variant_t pluginInstance;

    hr = type->get_IsAbstract(&isAbstract);
    if (FAILED(hr) || isAbstract) {
        return nullptr;
    }

    hr = pluginType->IsAssignableFrom(type, &isAssignable); 
    if (FAILED(hr) || !isAssignable) {
        return nullptr;
    }

    constructorArgs = SafeArrayCreateVector(VT_UNKNOWN, 0, 0);

    hr = type->GetConstructor_3(constructorArgs, &constructor);
    if (FAILED(hr) || constructor == nullptr) 
    { 
        Log(TEXT("Failed to create get valid constructor for type %s: 0x%08lx"), typeName.c_str(), hr); 
        goto errorCleanup; 
    }

    SafeArrayDestroy(constructorArgs);
    constructorArgs = nullptr;

    hr = constructor->Invoke_5(nullptr, &pluginInstance);
    if (FAILED(hr)) {
        Log(TEXT("Failed to create new instance of plugin type %s: 0x%08lx"), typeName.c_str(), hr); 
        goto errorCleanup;
    }

    constructor->Release();
    constructor = nullptr;

    CLRPlugin *plugin = new CLRPlugin();
    if (plugin->Attach(CLRObjectRef(pluginInstance.punkVal, nullptr),  pluginType)) {
        return plugin;
    } else {
        delete plugin;
        return nullptr;
    }

errorCleanup:
    if (constructorArgs) {
        SafeArrayDestroy(constructorArgs);
        constructorArgs = nullptr;
    }
    if (constructor) {
        constructor->Release();
        constructor = nullptr;
    }

    return nullptr;
}

void CLRHost::LoadPlugins()
{
    if (!isInitialized) {
        Log(TEXT("CLRHost::LoadPlugins() Runtime not initialized, examine log for cause"));
    }

    std::vector<std::wstring> files;
    GetFilesInDirectory(files, INTEROP_PATH TEXT("*.dll"), -1);
    HRESULT hr;

    for(auto i = files.begin(); i < files.end(); i++) {
        std::wstring &file = *i;
        file = file.substr(0, file.size() - 4);
        bstr_t bstrFile(file.c_str());
        _Assembly *pluginAssembly;
        SAFEARRAY *typeArray;
        _Type **types;

        Log(TEXT("CLRHost::LoadPlugins() attempting to load the plugin assembly %s"), file.c_str()); 
        hr = appDomain->Load_2(bstrFile, &pluginAssembly);
        if (FAILED(hr)) { 
            Log(TEXT("CLRHost::LoadPlugins() failed to load the assembly %s: 0x%08lx"), file.c_str(), hr); 
            goto errorCleanup;
        }

        hr = pluginAssembly->GetTypes(&typeArray);
        if (FAILED(hr)) {
            Log(TEXT("CLRHost::LoadPlugins() failed to retrieve exported types in assembly %s: 0x%08lx"), file.c_str(), hr);
            goto errorCleanup;
        }

        types = (_Type **)typeArray->pvData;

        bool hasPluginClass = false;
        for(ULONG i = 0; i < typeArray->rgsabound->cElements; i++) {
            _Type *type = types[i];
            CLRPlugin *plugin = CreatePluginInstance(file, type, pluginType, libraryType, libraryInstance);
            if (plugin && plugin->LoadPlugin()) {
                clrPlugins.push_back(plugin);
                
                BSTR bstrTypeName;
                type->get_ToString(&bstrTypeName);
                std::wstring typeName = bstrTypeName;
                ::SysFreeString(bstrTypeName);
                Log(TEXT("CLRHost::LoadPlugins() successfully added CLR plugin [Type: %s, Name: %s]"), typeName.c_str(), plugin->GetPluginName().c_str());
                hasPluginClass = true;
            }
        }
        if (!hasPluginClass) {
            Log(TEXT("CLRHost::LoadPlugins() no valid plugin types found in assembly %s"), file.c_str());
        }

        SafeArrayDestroy(typeArray);
errorCleanup:
        if (pluginAssembly) {
            pluginAssembly->Release();
        }
    }

}

void CLRHost::UnloadPlugins()
{
    while(clrPlugins.size()) {
        CLRPlugin *plugin = clrPlugins[0];
        plugin->UnloadPlugin();
        delete plugin;
        clrPlugins.erase(clrPlugins.begin());
    }
}

void CLRHost::OnStartStream()
{
    FOREACH_PLUGIN(OnStartStream());
}

void CLRHost::OnStopStream()
{
    FOREACH_PLUGIN(OnStopStream());
}

void CLRHost::OnStartStreaming()
{
    FOREACH_PLUGIN(OnStartStreaming());
}

void CLRHost::OnStopStreaming()
{
    FOREACH_PLUGIN(OnStopStreaming());
}

void CLRHost::OnStartRecording()
{
    FOREACH_PLUGIN(OnStartRecording());
}

void CLRHost::OnStopRecording()
{
    FOREACH_PLUGIN(OnStopRecording());
}

void CLRHost::OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting)
{
    FOREACH_PLUGIN(OnOBSStatus(running, streaming, recording, previewing, reconnecting));
}

void CLRHost::OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps)
{
    FOREACH_PLUGIN(OnStreamStatus(streaming, previewOnly, bytesPerSec, strain, totalStreamTime, totalNumFrames, numDroppedFrames, fps));
}

void CLRHost::OnSceneSwitch(CTSTR scene)
{
    FOREACH_PLUGIN(OnSceneSwitch(scene));
}

void CLRHost::OnScenesChanged()
{
    FOREACH_PLUGIN(OnScenesChanged());
}

void CLRHost::OnSourceOrderChanged()
{
    FOREACH_PLUGIN(OnSourceOrderChanged());
}

void CLRHost::OnSourceChanged(CTSTR sourceName, XElement* source)
{
    FOREACH_PLUGIN(OnSourceChanged(sourceName, source));
}

void CLRHost::OnSourcesAddedOrRemoved()
{
    FOREACH_PLUGIN(OnSourcesAddedOrRemoved());
}

void CLRHost::OnMicVolumeChanged(float level, bool muted, bool finalValue)
{
    FOREACH_PLUGIN(OnMicVolumeChanged(level, muted, finalValue));
}

void CLRHost::OnDesktopVolumeChanged(float level, bool muted, bool finalValue)
{
    FOREACH_PLUGIN(OnDesktopVolumeChanged(level, muted, finalValue));
}

void CLRHost::OnLogUpdate(CTSTR delta, UINT length)
{
    FOREACH_PLUGIN(OnLogUpdate(delta, length));
}
