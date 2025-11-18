#include "OddPlatformWindow.h"
#include "WeakObjPtr.h"

namespace Odd
{
    static WeakObjPtr<IWindow> GMainWindow;

    IWindow::IWindow()
    {
        // All other windows that get created should be attached to the main window.
        if (!GMainWindow.IsValid())
        {
            GMainWindow = this;
        }
        else
        {
            GMainWindow->Attach(this);
        }
    }
}