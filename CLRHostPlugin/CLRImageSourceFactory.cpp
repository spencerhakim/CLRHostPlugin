#include "Stdafx.h"
#include "CLRImageSourceFactory.h"

#pragma comment(lib, "mscoree.lib") 
#import "mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent") 
using namespace mscorlib; 

bool CLRImageSourceFactory::Attach(CLRObjectRef &clrObjectRef, mscorlib::_Type *objectType)
{
    CLRObject::Attach(clrObjectRef, objectType);

    bstr_t createMethodName("Create");
    bstr_t getDisplayNameMethodName("get_DisplayName");
    bstr_t getSourceClassNameMethodName("get_ClassName");
    bstr_t showConfigurationMethodName("ShowConfiguration");


    HRESULT hr;

    hr = objectType->GetMethod_6(createMethodName, &createMethod);
    if (FAILED(hr)) {
        Log(TEXT("Failed to get Create method definition of ImageSourceFactory class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = objectType->GetMethod_6(getDisplayNameMethodName, &getDisplayNameMethod);
    if (FAILED(hr)) {
        Log(TEXT("Failed to get GetDisplayName method definition of ImageSourceFactory class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = objectType->GetMethod_6(getSourceClassNameMethodName, &getSourceClassNameMethod);
    if (FAILED(hr)) {
        Log(TEXT("Failed to get GetSourceClassName method definition of ImageSourceFactory class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = objectType->GetMethod_6(showConfigurationMethodName, &showConfigurationMethod);
    if (FAILED(hr)) {
        Log(TEXT("Failed to get ShowConfiguration method definition of ImageSourceFactory class: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    goto success;

errorCleanup:

    Detach();
    return false;

success:
    return true;
}

void CLRImageSourceFactory::Detach()
{
    if (createMethod) {
        createMethod->Release();
        createMethod = nullptr;
    }
    if (getDisplayNameMethod) {
        getDisplayNameMethod->Release();
        getDisplayNameMethod = nullptr;
    }
    if (getSourceClassNameMethod) {
        getSourceClassNameMethod->Release();
        getSourceClassNameMethod = nullptr;
    }
    if (showConfigurationMethod) {
        showConfigurationMethod->Release();
        showConfigurationMethod = nullptr;
    }

    CLRObject::Detach();
}

CLRImageSource *CLRImageSourceFactory::Create(CLRXElement *element)
{
    if (!IsValid()) {
        Log(TEXT("CLRImageSourceFactory::Create() no managed object attached"));
        return nullptr;
    }

    // Must Release
    SAFEARRAY *createArgs = nullptr;
    SAFEARRAY *parameterTypes = nullptr;
    SAFEARRAY *constructors = nullptr;
    SAFEARRAY *constructorParameters = nullptr;
    _ParameterInfo *elementTypeInfo = nullptr;
    _Type *elementType = nullptr;
    _Type *returnType = nullptr;
    CLRImageSource *imageSource = nullptr;

    // Local
    HRESULT hr;
    variant_t objectRef(GetObjectRef());
    variant_t returnVal;
    variant_t elementRef(element->GetObjectRef());
    LONG argIndex = 0;
        
    createArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);

    hr = SafeArrayPutElement(createArgs, &argIndex, &elementRef); 
    if (FAILED(hr)) { 
        Log(TEXT("CLRImageSourceFactory::Create() failed to set config argument pointer: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = createMethod->Invoke_3(objectRef, createArgs, &returnVal);
    if (FAILED(hr) || !returnVal.punkVal) {
        Log(TEXT("Failed to invoke Create on managed instance: 0x%08lx"), hr); 
        goto errorCleanup;
    }
    SafeArrayDestroy(createArgs);
    createArgs = nullptr;

    hr = createMethod->get_returnType(&returnType);
    if (FAILED(hr) || !returnType) {
        Log(TEXT("Failed to get return type for Create method"));
        goto errorCleanup;
    }

    imageSource = new CLRImageSource();
    if (!imageSource->Attach(CLRObjectRef(returnVal.punkVal, nullptr), returnType)) {
        Log(TEXT("Failed to attach unmanaged wrapper to managed ImageSource object"));
        goto errorCleanup;
    }
    returnType->Release();
    goto success;

errorCleanup:
    if (parameterTypes) {
        SafeArrayDestroy(parameterTypes);
        parameterTypes = nullptr;
    }
    if (createArgs) {
        SafeArrayDestroy(createArgs);
        createArgs = nullptr;
    }
    if (constructors) {
        SafeArrayDestroy(constructors);
        constructors = nullptr;
    }
    if (constructorParameters) {
        SafeArrayDestroy(constructorParameters);
        constructorParameters = nullptr;
    }
    if (elementType) {
        elementType->Release();
        elementType = nullptr;
    }
    if (returnType) {
        returnType->Release();
        returnType = nullptr;
    }
    if (imageSource) {
        delete imageSource;
        imageSource = nullptr;
    }
success:

    return imageSource;   
}

std::wstring CLRImageSourceFactory::GetDisplayName()
{
    if (!IsValid()) {
        Log(TEXT("CLRImageSourceFactory::GetDisplayName() no managed object attached"));
        return std::wstring(TEXT("!error! see log"));
    }

    variant_t objectRef(GetObjectRef());
    variant_t returnVal;

    HRESULT hr = getDisplayNameMethod->Invoke_3(objectRef, nullptr, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke GetDisplayName on managed instance: 0x%08lx"), hr); 
        return std::wstring(TEXT("!error! see log"));
    }

    return std::wstring((const wchar_t*)returnVal.bstrVal);
}

std::wstring CLRImageSourceFactory::GetSourceClassName()
{
    if (!IsValid()) {
        Log(TEXT("CLRImageSourceFactory::GetSourceClassName() no managed object attached"));
        return std::wstring(TEXT("!error! see log"));
    }

    variant_t objectRef(GetObjectRef());
    variant_t returnVal;

    HRESULT hr = getSourceClassNameMethod->Invoke_3(objectRef, nullptr, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("Failed to invoke GetSourceClassName on managed instance: 0x%08lx"), hr); 
        return std::wstring(TEXT("!error! see log"));
    }

    return std::wstring((const wchar_t*)returnVal.bstrVal);
}

bool CLRImageSourceFactory::ShowConfiguration(CLRXElement *element)
{
    if (!IsValid()) {
        Log(TEXT("CLRImageSourceFactory::ShowConfiguration() no managed object attached"));
        return false;
    }
    HRESULT hr;            
    variant_t objectRef(GetObjectRef());
    SAFEARRAY *createArgs = nullptr;
    variant_t elementRef(element->GetObjectRef());
    LONG argIndex = 0;
    variant_t returnVal(false);

    createArgs = SafeArrayCreateVector(VT_VARIANT, 0, 1);

    hr = SafeArrayPutElement(createArgs, &argIndex, &elementRef); 
    if (FAILED(hr)) { 
        Log(TEXT("CLRImageSourceFactory::ShowConfiguration() failed to set config argument pointer: 0x%08lx"), hr); 
        goto errorCleanup;
    }

    hr = showConfigurationMethod->Invoke_3(objectRef, createArgs, &returnVal);
    if (FAILED(hr)) {
        Log(TEXT("CLRImageSourceFactory::ShowConfiguration() failed to invoke ShowConfiguration on managed instance: 0x%08lx"), hr); 
        goto errorCleanup;
    }
    SafeArrayDestroy(createArgs);
    createArgs = nullptr;

    goto success;

errorCleanup:
    if (createArgs) {
        SafeArrayDestroy(createArgs);
        createArgs = nullptr;
    }

success:
    return returnVal.boolVal == VARIANT_TRUE;
}