#include "Stdafx.h"
#include "CLRPlugin.h"

#pragma comment(lib, "mscoree.lib") 
#import "mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent") 
using namespace mscorlib;

#define GET_METHODINFO(x)                                                                   \
    hr = objectType->GetMethod_6(bstr_t(x), &methodInfo);                                   \
    if (FAILED(hr) || !methodInfo) {                                                        \
        Log(TEXT("Failed to get %s method definition of Plugin class: 0x%08lx"), x, hr);    \
        goto errorCleanup;                                                                  \
    }                                                                                       \
    methodInfos[x] = methodInfo;

#define SAFE_RELEASE(x) if(x){ x->Release(); x = nullptr; }
#define CLR_INVOKE(x) Invoke(__FUNCTION__, x)
#define PUT_ELEMENT(x, y) { LONG index[] = { x }; SafeArrayPutElement(args, index, &y); }

bool CLRPlugin::Attach(CLRObjectRef &clrObjectRef, mscorlib::_Type *objectType)
{
    CLRObject::Attach(clrObjectRef, objectType);

    mscorlib::_MethodInfo* methodInfo = NULL;
    HRESULT hr = S_OK;

    GET_METHODINFO("get_Name");
    GET_METHODINFO("get_Description");

    GET_METHODINFO("LoadPlugin");
    GET_METHODINFO("UnloadPlugin");

    GET_METHODINFO("OnStartStream");
    GET_METHODINFO("OnStopStream");

    goto success;

errorCleanup:
    Detach();
    return false;

success:
    return true;
}

void CLRPlugin::Detach()
{
    for (auto i = methodInfos.begin(); i != methodInfos.end(); i++)
        SAFE_RELEASE(i->second);

    CLRObject::Detach();
}

void CLRPlugin::Invoke(std::string funcMacro, LPSAFEARRAY parameters)
{
    if (!IsValid()) {
        Log(TEXT("%s() no managed object attached"), funcMacro);
    }

    variant_t objectRef(GetObjectRef());

    auto splitMacro = split(funcMacro, "::");
    HRESULT hr = methodInfos[splitMacro[splitMacro.size()-1]]->Invoke_3(objectRef, parameters, nullptr);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke %s on managed instance: 0x%08lx"), funcMacro, hr);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring CLRPlugin::GetPluginName()
{
    if (!IsValid()) {
        Log(TEXT("CLRPlugin::GetPluginName() no managed object attached"));
        return std::wstring(TEXT("!error! see log"));
    }

    variant_t objectRef(GetObjectRef());
    variant_t returnVal;

    HRESULT hr = methodInfos["get_Name"]->Invoke_3(objectRef, nullptr, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke GetPluginName on managed instance: 0x%08lx"), hr);
        return std::wstring(TEXT("!error! see log"));
    }

    return std::wstring((const wchar_t*)returnVal.bstrVal);
}

std::wstring CLRPlugin::GetPluginDescription()
{
    if (!IsValid()) {
        Log(TEXT("CLRPlugin::GetPluginDescription() no managed object attached"));
        return std::wstring(TEXT("!error! see log"));
    }

    variant_t objectRef(GetObjectRef());
    variant_t returnVal;

    HRESULT hr = methodInfos["get_Description"]->Invoke_3(objectRef, nullptr, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke GetPluginDescription on managed instance: 0x%08lx"), hr);
        return std::wstring(TEXT("!error! see log"));
    }

    return std::wstring((const wchar_t*)returnVal.bstrVal);
}

bool CLRPlugin::LoadPlugin()
{
    if (!IsValid()) {
        Log(TEXT("CLRPlugin::OnStopStream() no managed object attached"));
    }

    variant_t objectRef(GetObjectRef());
    variant_t returnVal;

    HRESULT hr = methodInfos["LoadPlugin"]->Invoke_3(objectRef, nullptr, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke OnStopStream on managed instance: 0x%08lx"), hr); 
    }

    return returnVal.boolVal == VARIANT_TRUE;
}

void CLRPlugin::UnloadPlugin()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStartStream()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStopStream()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStartStreaming()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStopStreaming()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStartRecording()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnStopRecording()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting)
{
    SAFEARRAYBOUND bounds[] = { { 5, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, running);
    PUT_ELEMENT(1, streaming);
    PUT_ELEMENT(2, recording);
    PUT_ELEMENT(3, previewing);
    PUT_ELEMENT(4, reconnecting);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnStreamStatus(bool streaming, bool previewOnly, UINT bytesPerSec, double strain, UINT totalStreamTime, UINT totalNumFrames, UINT numDroppedFrames, UINT fps)
{
    SAFEARRAYBOUND bounds[] = { { 8, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, streaming);
    PUT_ELEMENT(1, previewOnly);
    PUT_ELEMENT(2, bytesPerSec);
    PUT_ELEMENT(3, strain);
    PUT_ELEMENT(4, totalStreamTime);
    PUT_ELEMENT(5, totalNumFrames);
    PUT_ELEMENT(6, numDroppedFrames);
    PUT_ELEMENT(7, fps);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnSceneSwitch(CTSTR scene)
{
    SAFEARRAYBOUND bounds[] = { { 1, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, scene);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnScenesChanged()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnSourceOrderChanged()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnSourceChanged(CTSTR sourceName, XElement* source)
{
    CLRHost* clrHost = CLRHostPlugin::instance->GetCLRHost();
    CLRXElement* clrSource = CLRXElement::Create(clrHost->GetXElementType(), source);

    SAFEARRAYBOUND bounds[] = { { 2, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, sourceName);
    PUT_ELEMENT(1, *clrSource);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnSourcesAddedOrRemoved()
{
    CLR_INVOKE(nullptr);
}

void CLRPlugin::OnMicVolumeChanged(float level, bool muted, bool finalValue)
{
    SAFEARRAYBOUND bounds[] = { { 3, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, level);
    PUT_ELEMENT(1, muted);
    PUT_ELEMENT(2, finalValue);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnDesktopVolumeChanged(float level, bool muted, bool finalValue)
{
    SAFEARRAYBOUND bounds[] = { { 3, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, level);
    PUT_ELEMENT(1, muted);
    PUT_ELEMENT(2, finalValue);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}

void CLRPlugin::OnLogUpdate(CTSTR delta, UINT length)
{
    SAFEARRAYBOUND bounds[] = { { 2, 0 } };
    LPSAFEARRAY args = SafeArrayCreate(VT_VARIANT, 1, bounds);
    PUT_ELEMENT(0, delta);
    PUT_ELEMENT(1, length);

    CLR_INVOKE(args);

    SafeArrayDestroy(args);
}