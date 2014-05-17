#include "Stdafx.h"
#include "CLRObject.h"

#pragma comment(lib, "mscoree.lib") 
#import "mscorlib.tlb" raw_interfaces_only high_property_prefixes("_get","_put","_putref") rename("ReportEvent", "InteropServices_ReportEvent") 
using namespace mscorlib; 

CLRObject::~CLRObject() {
    if (objectType && objectRef) {
        IDisposable *disposable;
        HRESULT hr = objectRef->QueryInterface(&disposable);
        if (SUCCEEDED(hr) && disposable) {
            disposable->Dispose();
            disposable->Release();
        }
    }
    Detach();
}

bool CLRObject::Attach(CLRObjectRef &clrObjectRef, mscorlib::_Type *objectType)
{
    this->objectRef = (IUnknown *)clrObjectRef.GetObjectRef();
    this->objectType = objectType;

    objectRef->AddRef();
    objectType->AddRef();
    return true;
}

void CLRObject::Detach()
{
    if (objectRef) {
        objectRef->Release();
        objectRef = nullptr;
    }
    if (objectType) {
        objectType->Release();
        objectType = nullptr;
    }
}
