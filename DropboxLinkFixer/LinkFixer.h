/////////////////////////////////
// Created by Connor Hilarides //
// 2014-07-09                  //
/////////////////////////////////

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <Windows.h>

extern "C"
{
  void RegisterClipboardUpdate(HWND hWnd);
  LRESULT CALLBACK ClipboardUpdateCallback(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);
  void FixClipboard(HWND hwnd);
  HWND MakeInvisibleWindow(HINSTANCE hInstance);
  void HandleWindow(HWND hwnd);
}
