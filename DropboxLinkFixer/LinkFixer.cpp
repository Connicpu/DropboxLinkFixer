/////////////////////////////////
// Created by Connor Hilarides //
// 2014-07-09                  //
/////////////////////////////////

#include "LinkFixer.h"
#include <string>

class ClipboardViewer final
{
public:
  ClipboardViewer(HWND hwnd);
  ~ClipboardViewer();

  bool NeedsFixing();

  ClipboardViewer& operator=(const std::string& newData);

  template <typename T>
  inline T *GetData() { return reinterpret_cast<T *>(data); }

private:
  HANDLE hClipboard;
  void *data;
};

const std::string replaceText = "https://www.dropbox.com/s/";
const std::string newText = "https://dl.dropboxusercontent.com/s/";

extern "C"
{
  void RegisterClipboardUpdate(HWND hWnd)
  {
    AddClipboardFormatListener(hWnd);
  }

  LRESULT CALLBACK ClipboardUpdateCallback(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
  {
    if (msg == WM_CLIPBOARDUPDATE)
    {
      FixClipboard(hwnd);
      return 0;
    }

    return DefWindowProc(hwnd, msg, wp, lp);
  }

  void FixClipboard(HWND hwnd)
  {
    ClipboardViewer clipboard{hwnd};

    if (!clipboard.NeedsFixing())
      return;

    std::string text = clipboard.GetData<char>();
    text.replace(text.begin(), text.begin() + replaceText.length(), newText);

    clipboard = text;
  }
  
  HWND MakeInvisibleWindow(HINSTANCE hInstance)
  {
    WNDCLASSEX wndc = {sizeof(wndc)};
    wndc.hInstance = hInstance;
    wndc.lpfnWndProc = ClipboardUpdateCallback;
    wndc.lpszClassName = "Invisible Window *spooky face*";
    RegisterClassEx(&wndc);

    auto window = CreateWindow(wndc.lpszClassName, "Invisible Window :o", 
                               0, 0, 0, 0, 0, HWND_MESSAGE, 0, hInstance, 0);
    RegisterClipboardUpdate(window);

    return window;
  }

  void HandleWindow(HWND hwnd)
  {
    MSG msg;
    while (GetMessage(&msg, hwnd, 0, 0))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}

ClipboardViewer::ClipboardViewer(HWND hwnd)
  : hClipboard(nullptr), data(nullptr)
{
  OpenClipboard(hwnd);
  hClipboard = GetClipboardData(CF_TEXT);

  if (hClipboard != nullptr)
    data = GlobalLock(hClipboard);
}

ClipboardViewer::~ClipboardViewer()
{
  if (data != nullptr)
    GlobalUnlock(hClipboard);

  CloseClipboard();
}

bool ClipboardViewer::NeedsFixing()
{
  if (hClipboard == nullptr)
    return false;

  std::string text = GetData<char>();
  return text.find(replaceText) == 0;
}

ClipboardViewer& ClipboardViewer::operator=(const std::string& newData)
{
  size_t size = (newData.length() + 1) * sizeof(char);
  HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, size);
  LPWSTR cbString = reinterpret_cast<LPWSTR>(GlobalLock(hMem));
  memcpy_s(cbString, size, newData.c_str(), size);
  GlobalUnlock(hMem);
  
  EmptyClipboard();
  SetClipboardData(CF_TEXT, hMem);

  return *this;
}
