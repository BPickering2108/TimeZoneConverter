#include <windows.h>
#include <tchar.h>
#include <string>
#include "TimeConverter.h"
#include "resource.h"

#define ID_EDIT_TIME 101
#define ID_COMBO_ZONE 102
#define ID_CHECK_GMT 103
#define ID_BUTTON_CONVERT 104
#define ID_LABEL_RESULT 105

HWND hEditTime, hComboZone, hCheckGMT, hButtonConvert, hLabelResult;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Time Input Field (Edit Control) - Restricted to HH:MM Format
        hEditTime = CreateWindowEx(0, _T("EDIT"), _T("00:00"),
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
            10, 10, 80, 25, hWnd, (HMENU)ID_EDIT_TIME, NULL, NULL);

        // Time Zone Dropdown (ComboBox)
        hComboZone = CreateWindowEx(0, _T("COMBOBOX"), NULL,
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
            100, 10, 150, 200, hWnd, (HMENU)ID_COMBO_ZONE, NULL, NULL);
        SendMessage(hComboZone, CB_ADDSTRING, 0, (LPARAM)_T("UTC"));
        SendMessage(hComboZone, CB_ADDSTRING, 0, (LPARAM)_T("EST"));
        SendMessage(hComboZone, CB_ADDSTRING, 0, (LPARAM)_T("PST"));
        SendMessage(hComboZone, CB_ADDSTRING, 0, (LPARAM)_T("CET"));
        SendMessage(hComboZone, CB_ADDSTRING, 0, (LPARAM)_T("IST"));

        SendMessage(hComboZone, CB_SETCURSEL, 0, 0); // Select "UTC" by default

        // GMT/BST Checkbox
        hCheckGMT = CreateWindowEx(0, _T("BUTTON"), _T("Use DST"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            10, 45, 80, 25, hWnd, (HMENU)ID_CHECK_GMT, NULL, NULL);

        // Convert Button
        hButtonConvert = CreateWindowEx(0, _T("BUTTON"), _T("Convert Time"),
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            110, 45, 100, 30, hWnd, (HMENU)ID_BUTTON_CONVERT, NULL, NULL);

        // Result Label
        hLabelResult = CreateWindowEx(0, _T("STATIC"), _T("Converted Time: --:--"),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            10, 90, 250, 100, hWnd, (HMENU)ID_LABEL_RESULT, NULL, NULL);
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == ID_BUTTON_CONVERT)
        {
            ConvertTime(hEditTime, hComboZone, hLabelResult, hCheckGMT);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0,
        hInstance, LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
        (HBRUSH)(COLOR_WINDOW + 1), NULL, _T("TimeZoneConverter"), LoadIcon(NULL, IDI_APPLICATION) 
    };

    wcex.hIcon = (HICON)LoadImage(hInstance, _T("timeconvert.ico"), IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
    wcex.hIconSm = (HICON)LoadImage(hInstance, _T("timeconvert.ico"), IMAGE_ICON, 16, 16, LR_LOADFROMFILE);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Window registration failed!"), _T("Error"), MB_OK | MB_ICONERROR);
        return 1;
    }

    wcex.lpszClassName = _T("TimeZoneConverter");
    HWND hWnd = CreateWindowEx(0, _T("TimeZoneConverter"), _T("Time Zone Converter"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 250, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, _T("Window creation failed!"), _T("Error"), MB_OK | MB_ICONERROR);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)));
    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP_ICON)));

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}