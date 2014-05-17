#pragma once

#include "CLRObject.h"

namespace mscorlib {
    struct _Type;
    struct _MethodInfo;
}

class CLRXElement : public CLRObject
{
public:
    static CLRXElement *Create(mscorlib::_Type *type, void *element);

};