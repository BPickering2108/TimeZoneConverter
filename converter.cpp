#include "TimeConverter.h"
#include <string>

void ConvertTime(HWND hEdit, HWND hCombo, HWND hLabel, HWND hCheck)
{
    // Get user-entered time
    TCHAR timeBuffer[6];  // HH:MM format
    GetWindowText(hEdit, timeBuffer, 6);

    int hours, minutes;
    if (_stscanf_s(timeBuffer, _T("%d:%d"), &hours, &minutes) != 2 || hours < 0 || hours > 23 || minutes < 0 || minutes > 59)
    {
        MessageBox(NULL, _T("Invalid time format. Use HH:MM."), _T("Error"), MB_OK | MB_ICONERROR);
        return;
    }

    // Get selected time zone from drop-down
    int selectedIndex = SendMessage(hCombo, CB_GETCURSEL, 0, 0);
    int baseOffset = 0;

    if (selectedIndex == 0) baseOffset = 0;  // UTC
    else if (selectedIndex == 1) baseOffset = -5;  // EST
    else if (selectedIndex == 2) baseOffset = -8;  // PST
    else if (selectedIndex == 3) baseOffset = +1;  // CET
    else if (selectedIndex == 4) baseOffset = +5.5; // IST

    bool isBST = SendMessage(hCheck, BM_GETCHECK, 0, 0) == BST_CHECKED;

    // Only adjust BST for applicable time zones
    if (isBST && (selectedIndex == 0 || selectedIndex == 3)) {
        baseOffset += 1;  // BST applies only to UTC & CET
    }

    // Define time zones with offsets
    struct TimeZone {
        const TCHAR* name;
        int offset;
    };

    TimeZone zones[] = {
        { _T("UTC"), 0 },
        { _T("EST"), -5 },
        { _T("PST"), -8 },
        { _T("CET"), +1 },
        { _T("IST"), +5.5}
    };

    // Generate result string for all time zones
    TCHAR resultBuffer[800] = _T("Converted Times:\n");

    for (const auto& zone : zones) {
        int convertedHours = (hours + (zone.offset - baseOffset)) % 24;
        int convertedMinutes = minutes; // Default: no minute change

        // Apply DST adjustment to UTC and CET
        if (isBST && (_tcscmp(zone.name, _T("UTC")) == 0 || _tcscmp(zone.name, _T("CET")) == 0)) {
            convertedHours = (convertedHours + 1) % 24;
        }

        // Adjust minutes for IST (UTC+5:30)
        if (zone.offset == 5.5) {
            convertedMinutes += 30; // Add 30 minutes

            if (convertedMinutes >= 60) {
                convertedMinutes -= 60;
                convertedHours = (convertedHours + 1) % 24;
            }
        }

        // Ensure hour wraps correctly for negative time zones
        if (convertedHours < 0) convertedHours += 24;

        TCHAR tempBuffer[50];
        _stprintf_s(tempBuffer, _T("%s: %02d:%02d\n"), zone.name, convertedHours, convertedMinutes);
        //if (_tcscmp(zone.name, _T("IST")) == 0) {
        //    MessageBox(NULL, _T("IST is being processed"), _T("Debug"), MB_OK);
        //}
        _tcscat_s(resultBuffer, tempBuffer);
    }

    SetWindowText(hLabel, resultBuffer);
}