#pragma once

#include "ImageSourceFactory.h"
#include "ImageSource.h"
#include "XElement.h"

using namespace System;
using namespace System::Windows;
using namespace System::Windows::Interop;

namespace CLROBS 
{
    public ref class AbstractImageSourceFactory abstract : public ImageSourceFactory
    {
    private:
        System::String^ className;
        System::String^ displayName;

    public:
        AbstractImageSourceFactory();

    public: // ImageSourceFactory

        virtual ImageSource^ Create(XElement^ data) abstract;
        virtual bool ShowConfiguration(XElement^ data) abstract;

        property System::String^ DisplayName
        {
        public:
            virtual System::String^ get() sealed;
        protected:
            void set(System::String^ displayName);
        }

        property System::String^ ClassName
        {
        public:
            virtual System::String^ get() sealed;
        protected:
            void set(System::String^ className);
        }  
    };
};