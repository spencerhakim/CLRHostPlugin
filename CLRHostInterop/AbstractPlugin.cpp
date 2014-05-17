#include "Stdafx.h"
#include "AbstractPlugin.h"

namespace CLROBS {
    AbstractPlugin::AbstractPlugin()
    {
        Name = gcnew System::String("Default Plugin Name");
        Description = gcnew System::String("Default Plugin Description");
    }

    AbstractPlugin::~AbstractPlugin()
    {
    }

    System::String^ AbstractPlugin::Name::get()
    {
        return pluginName;
    }

    void AbstractPlugin::Name::set(System::String^ pluginName)
    {
        this->pluginName = pluginName;
    }

    System::String^ AbstractPlugin::Description::get()
    {
        return pluginDescription;
    }

    void AbstractPlugin::Description::set(System::String^ pluginDescription)
    {
        this->pluginDescription = pluginDescription;
    }
}