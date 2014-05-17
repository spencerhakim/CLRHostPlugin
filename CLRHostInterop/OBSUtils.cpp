#include "Stdafx.h"
#include "OBSUtils.h"

#include <msclr\marshal_cppstd.h>

std::wstring ToWString(System::String^ string)
{
    return msclr::interop::marshal_as<std::wstring, System::String^>(string);
}
