#include "Stdafx.h"
#include "AbstractWPFSettingsPane.h"

using namespace System::Windows::Controls;

IntPtr ChildHwndSourceHook(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, bool% handled)
{
   if (msg == _WM_GETDLGCODE)
   {
      handled = true;
      return IntPtr(_DLGC_WANTALLKEYS);
   }
 
   return IntPtr(0);
}

namespace CLROBS {
    AbstractWPFSettingsPane::AbstractWPFSettingsPane()
    {
        this->Category = gcnew System::String("DefaultSettingsPanelCategory");
    }

    long long AbstractWPFSettingsPane::CreatePane(long long parentHwnd)
    {
        hwndSource = gcnew HwndSource(
            0,                                      // class style
            _DS_CONTROL | _WS_CHILD | _WS_SYSMENU,  // style
            _WS_EX_CONTROLPARENT,                   // exstyle
            0, 0, (int)(427 * 1.5), (int)(336 * 1.5), // why do I have to do this?
            "Helllo!",
            IntPtr(parentHwnd)                      // parent window
            );

        // hook all keyboard keys
        // in the future make it interact better (don't capture esc, etc.)
        hwndSource->AddHook(gcnew HwndSourceHook(ChildHwndSourceHook));

        hwndSource->RootVisual = CreateUIElement();
        hwndSource->CompositionTarget->BackgroundColor = SystemColors::ControlColor;

        return hwndSource->Handle.ToInt64();
    }

    void AbstractWPFSettingsPane::DestroyPane()
    {
        if (hwndSource != nullptr) {
            delete hwndSource;
        }
    }

    System::String^ AbstractWPFSettingsPane::Category::get()
    {
        return category;
    }

    void AbstractWPFSettingsPane::Category::set(System::String^ category)
    {
        this->category = category;
    }
}