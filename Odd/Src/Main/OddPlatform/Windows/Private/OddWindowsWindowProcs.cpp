#include "OddWindowsWindowProcs.h"
#include "OddCore.h"
#include "OddWindowsWindow.h"

namespace Odd::Windows
{
    static WindowsWindow* GetWindowFromHWND(HWND hWnd)
    {
        LONG_PTR ptr = ::GetWindowLongPtrA(hWnd, GWLP_USERDATA);
        return rcast(WindowsWindow*, ptr);
    }

    LRESULT CALLBACK OddDefaultWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        switch (uMsg)
        {
            case WM_CREATE:
            {
                CREATESTRUCT* pCreateStruct = rcast(::CREATESTRUCT*, lParam);
                oddValidateMsg(pCreateStruct != nullptr, "WM_CREATE received with null CREATESTRUCT.");

                WindowsWindow* pWindow = scast(WindowsWindow*, pCreateStruct->lpCreateParams);
                oddValidateMsg(pWindow != nullptr, "WM_CREATE received with null WindowsWindow pointer.");

                ::SetWindowLongPtrA(hWnd, GWLP_USERDATA, LONG_PTR(pWindow));
                return 0;
            }
            case WM_CLOSE:
            {
                WindowsWindow* pWindow = GetWindowFromHWND(hWnd);
                oddValidateMsg(pWindow != nullptr, "WM_CLOSE received with null WindowsWindow pointer.");

                // Figure out what to do on close.
                pWindow->OnCloseMessage_Internal();
                return 0;
            }
            default:
                return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
        }
    }
} // namespace Odd::Windows