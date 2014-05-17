#pragma once

#include "XElement.h"

using namespace System;

namespace CLROBS {
    public interface class Plugin
    {
        property System::String^ Name {
            virtual System::String^ get();
        }
        property System::String^ Description {
            virtual System::String^ get();
        }

        virtual bool LoadPlugin() = 0;
        virtual void UnloadPlugin() = 0;

        virtual void OnStartStream() = 0;
        virtual void OnStopStream() = 0;

        virtual void OnStartStreaming() = 0;
        virtual void OnStopStreaming() = 0;

        virtual void OnStartRecording() = 0;
        virtual void OnStopRecording() = 0;

        virtual void OnOBSStatus(bool running, bool streaming, bool recording, bool previewing, bool reconnecting) = 0;
        virtual void OnStreamStatus(bool streaming, bool previewOnly, unsigned int bytesPerSec, double strain,
            unsigned int totalStreamTime, unsigned int totalNumFrames, unsigned int numDroppedFrames, unsigned int fps) = 0;

        virtual void OnSceneSwitch(System::String^ scene) = 0;
        virtual void OnScenesChanged() = 0;

        virtual void OnSourceOrderChanged() = 0;
        virtual void OnSourceChanged(System::String^ sourceName, XElement^ source) = 0;
        virtual void OnSourcesAddedOrRemoved() = 0;

        virtual void OnMicVolumeChanged(float level, bool muted, bool finalValue) = 0;
        virtual void OnDesktopVolumeChanged(float level, bool muted, bool finalValue) = 0;

        virtual void OnLogUpdate(System::String^ delta, unsigned int length) = 0;
    };
};