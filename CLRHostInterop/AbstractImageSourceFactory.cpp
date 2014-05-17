#include "Stdafx.h"
#include "AbstractImageSourceFactory.h"

#define WS_CHILD    0x40000000L
#define WS_VISIBLE  0x10000000L

namespace CLROBS {
    AbstractImageSourceFactory::AbstractImageSourceFactory()
    {
        this->ClassName = gcnew System::String("DefaultImageSourceClassName");
        this->DisplayName = gcnew System::String("DefaultImageSourceDisplayName");
    }

    System::String^ AbstractImageSourceFactory::ClassName::get()
    {
        return className;
    }

    void AbstractImageSourceFactory::ClassName::set(System::String^ className)
    {
        this->className = className;
    }

    System::String^ AbstractImageSourceFactory::DisplayName::get()
    {
        return displayName;
    }

    void AbstractImageSourceFactory::DisplayName::set(System::String^ displayName)
    {
        this->displayName = displayName;
    }
}