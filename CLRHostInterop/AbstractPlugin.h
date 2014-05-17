#pragma once

#include "Plugin.h"
#include "XElement.h"

namespace CLROBS 
{
    public ref class AbstractPlugin abstract : public Plugin
    {

    private:
        System::String^ pluginName;
        System::String^ pluginDescription;

    public:
        AbstractPlugin();
        ~AbstractPlugin();
    public:
        property System::String^ Name
        {
        public:
            virtual System::String^ get() sealed;
        protected:
            void set(System::String^ name);
        }
        property System::String^ Description
        {
        public:
            virtual System::String^ get() sealed;
        protected:
            void set(System::String^ description);
        }

        virtual bool LoadPlugin() { return true; }
        virtual void UnloadPlugin() {}

        virtual void OnStartStream() {}
        virtual void OnStopStream() {}

        virtual void OnStartStreaming() {}
        virtual void OnStopStreaming() {}

        virtual void OnStartRecording() {}
        virtual void OnStopRecording() {}

        virtual void OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting) {}
        virtual void OnStreamStatus(bool streaming, bool previewOnly, unsigned int bytesPerSec, double strain,
            unsigned int totalStreamTime, unsigned int totalNumFrames, unsigned int numDroppedFrames, unsigned int fps) {}

        virtual void OnSceneSwitch(System::String^ scene) {}
        virtual void OnScenesChanged() {}

        virtual void OnSourceOrderChanged() {}
        virtual void OnSourceChanged(System::String^ sourceName, XElement^ source) {}
        virtual void OnSourcesAddedOrRemoved() {}

        virtual void OnMicVolumeChanged(float level, bool muted, bool finalValue) {}
        virtual void OnDesktopVolumeChanged(float level, bool muted, bool finalValue) {}

        virtual void OnLogUpdate(System::String^ delta, unsigned int length) {}
    };
}