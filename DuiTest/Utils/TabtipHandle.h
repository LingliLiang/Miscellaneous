#pragma once

void AdviseinputPane(HWND hWnd);

void UnAdviseinputPane();

BOOL IsTabtipVisible();

// Show or Hide touch keyboard in win10?
// this call include function "IsVirtualKeyboardVisible", no need call that again.
int TabtipVisible(BOOL bShow);

// Hide touch keyboard
void TabtipTurnOff();

///RegFuctions
///most of fuctions take effect immediately,when keyboard state changed(hide->show)
///need infomation,check usage comments

void TabtipDock(BOOL bDock);

void TabtipCompatibilityKeyboard(BOOL bEnable);

void TabtipDesktopAutoShow(BOOL bEnable);

void TabtipAudioFeedback(BOOL bEnable);

void TabtipPasswordSecurity(DWORD dwSecurityLevel);

void TabtipEmoji();

void TabtipLayoutPreference(DWORD dwPreference);

void TabtipOneHandedKeyboardXPos(DWORD dwPos);

void TabtipOneHandedKeyboardYPos(DWORD dwPos);

void TabtipKeyboardXPos(DWORD dwPos);

void TabtipKeyboardYPos(DWORD dwPos);

void TabtipThumbKeyboardYPos(DWORD dwPos);

void TabtipThumbKeyboardSize(DWORD dwSize);

void TabtipTaskbarVisibility(BOOL bVisible);

void TabtipTicDocked(DWORD dw);

void TabtipTotalFinalizedCountForSIUF(DWORD dw);
