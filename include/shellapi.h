#ifndef _WINE_SHELLAPI_H
#define _WINE_SHELLAPI_H

#include "windef.h"

HICON16     WINAPI ExtractIcon16(HINSTANCE16,LPCSTR,UINT16);
HICON     WINAPI ExtractIconA(HINSTANCE,LPCSTR,UINT);
HICON     WINAPI ExtractIconW(HINSTANCE,LPCWSTR,UINT);
#define     ExtractIcon WINELIB_NAME_AW(ExtractIcon)
HICON16     WINAPI ExtractAssociatedIcon16(HINSTANCE16,LPSTR,LPWORD);
HICON     WINAPI ExtractAssociatedIconA(HINSTANCE,LPSTR,LPWORD);
HICON     WINAPI ExtractAssociatedIconW(HINSTANCE,LPWSTR,LPWORD);
#define     ExtractAssociatedIcon WINELIB_NAME_AW(ExtractAssociatedIcon)
HINSTANCE16 WINAPI FindExecutable16(LPCSTR,LPCSTR,LPSTR);
HINSTANCE WINAPI FindExecutableA(LPCSTR,LPCSTR,LPSTR);
HINSTANCE WINAPI FindExecutableW(LPCWSTR,LPCWSTR,LPWSTR);
#define     FindExecutable WINELIB_NAME_AW(FindExecutable)
BOOL16      WINAPI ShellAbout16(HWND16,LPCSTR,LPCSTR,HICON16);
BOOL      WINAPI ShellAboutA(HWND,LPCSTR,LPCSTR,HICON);
BOOL      WINAPI ShellAboutW(HWND,LPCWSTR,LPCWSTR,HICON);
#define     ShellAbout WINELIB_NAME_AW(ShellAbout)
HINSTANCE16 WINAPI ShellExecute16(HWND16,LPCSTR,LPCSTR,LPCSTR,LPCSTR,INT16);
HINSTANCE WINAPI ShellExecuteA(HWND,LPCSTR,LPCSTR,LPCSTR,LPCSTR,INT);
HINSTANCE WINAPI ShellExecuteW(HWND,LPCWSTR,LPCWSTR,LPCWSTR,LPCWSTR,INT);
#define     ShellExecute WINELIB_NAME_AW(ShellExecute)


#endif /* _WINE_SHELLAPI_H */
