#include "resource.h"

// int WINAPI WinMain(HANDLE, HANDLE, LPSTR, int); defined in windows header
BOOL InitApplication(HANDLE);
BOOL InitInstance(HINSTANCE, int);
long CALLBACK  MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK About(HWND,   UINT, WPARAM, LPARAM );
BOOL CALLBACK Options(HWND, UINT, WPARAM, LPARAM );
