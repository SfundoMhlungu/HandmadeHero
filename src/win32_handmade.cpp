
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <windows.h>
#include <xinput.h>
#define internal static
#define local_persist static
#define global_var static

typedef uint8_t uint8;
typedef uint32_t uint32;
global_var bool Running;
global_var BITMAPINFO Bitmapinfo;
global_var void *BitmapMemory;
global_var int bitMapwidth;
global_var int bitMapheight;
global_var int BytesPerPixel = 4;

internal void renderWeirdGradient(int Xoffset, int Yoffset) {

  int width = bitMapwidth;

  int Pitch = width * BytesPerPixel;
  uint8 *Row = (uint8 *)BitmapMemory;
  for (int Y = 0; Y < bitMapheight; ++Y) {
    uint8 *Pixel = (uint8 *)Row;
    for (int X = 0; X < bitMapwidth; ++X) {
      // RGB IS BGRXX in memory day 4, 38 minutes explanation
      // iN MEMORY pixel : 00 00 00 00
      // BB GG RR XX

      // BLUE
      *Pixel = (uint8)(Y + Yoffset);
      ++Pixel;
      // GREEN
      *Pixel = (uint8)(X + Xoffset);
      ++Pixel;
      // red
      *Pixel = 0;
      ++Pixel;

      *Pixel = 0;
      ++Pixel;
    }

    Row += Pitch;
  }
}
internal void win32ResizeDIBSection(int width, int height) {
  // TODO(sk): maybe delete after new creation
  if (BitmapMemory) {
    // TODO(): virtual protect, make sure this space is not used, allowing code
    // to touch etc
    VirtualFree(BitmapMemory, 0, MEM_RELEASE);
  }

  bitMapwidth = width;
  bitMapheight = height;
  Bitmapinfo.bmiHeader.biSize = sizeof(Bitmapinfo.bmiHeader);

  Bitmapinfo.bmiHeader.biWidth = bitMapwidth;
  // positive height, the bit "draw" starts bottom left
  // negative top right(may have perfomance issues?)
  Bitmapinfo.bmiHeader.biHeight = -bitMapheight;
  Bitmapinfo.bmiHeader.biPlanes = 1;

  // 8 bit for r, 8 for b, 8 for gree
  Bitmapinfo.bmiHeader.biBitCount = 32;
  // no compression, to write to it fatser
  Bitmapinfo.bmiHeader.biCompression = BI_RGB;
  // no need static global will make zero for us
  // Bitmapinfo.bmiHeader.biSizeImage = 0;
  // Bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
  // Bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
  // Bitmapinfo.bmiHeader.biClrUsed = 0;
  // Bitmapinfo.bmiHeader.biClrImportant = 0

  int BitmapMemorySize = (bitMapwidth * bitMapheight) * BytesPerPixel;
  BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

  // renderWeirdGradient(0, 0);
};
internal void win32UpdateWindow(HDC DeviceContext, RECT ClientRect, int x,
                                int y, int width, int height) {

  int windowWidth = ClientRect.right - ClientRect.left;
  int windowHeight = ClientRect.bottom - ClientRect.top;
  StretchDIBits(DeviceContext, 0, 0, bitMapwidth, bitMapheight, 0, 0,
                windowWidth, windowHeight, BitmapMemory, &Bitmapinfo,
                DIB_RGB_COLORS, SRCCOPY);
};
LRESULT CALLBACK MainWindowCallBack(HWND window, UINT message, WPARAM WParam,
                                    LPARAM LParam) {

  // messsages are sent by windows for us to handles things like toolboxes
  // ect, all we need for us are just general(keybord etc) and are
  // under WM hex
  //
  LRESULT Result = 0; // for all the messages we handle
  switch (message) {
  case WM_ACTIVATEAPP: {
    std::cout << "ACTIVATE APP" << std::endl;
  } break;
  case WM_SIZE: {

    RECT ClientRect;
    GetClientRect(window, &ClientRect);
    int Height = ClientRect.bottom - ClientRect.top;
    int Width = ClientRect.right - ClientRect.left;
    win32ResizeDIBSection(Width, Height);
    std::cout << "size" << std::endl;
  } break;
  case WM_DESTROY: {
    // TODO(sk): probably an Error,
    Running = false;
    std::cout << "Destroy" << std::endl;
  } break;
  case WM_CLOSE: {
    // TODO(sk): ask are you sure you want to?
    Running = false;
    std::cout << "close" << std::endl;
  } break;
  case WM_PAINT: {
    PAINTSTRUCT Paint;
    HDC DeviceContext = BeginPaint(window, &Paint);
    int x = Paint.rcPaint.left;
    int y = Paint.rcPaint.top;
    int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
    int Width = Paint.rcPaint.right - Paint.rcPaint.left;
    RECT ClientRect_;
    GetClientRect(window, &ClientRect_);

    win32UpdateWindow(DeviceContext, ClientRect_, x, y, Width, Height);
    EndPaint(window, &Paint);
  } break;
  default:
    // std::cout << "UNHANDLED" << std::endl;
    // we call DefWindowProc to handle all the message we dont care about, and
    // will return the relevant LRESULT
    { Result = DefWindowProc(window, message, WParam, LParam); }
    break;
  }
  return (Result);
};

int APIENTRY WinMain(HINSTANCE Instance, HINSTANCE PrevInstance,
                     PSTR CommandLine, int showCode) {

  DWORD dwResult;

  WNDCLASS windowClass = {};
  //
  // typedef struct tagWNDCLASSA {
  //   UINT style;
  //   WNDPROC lpfnWndProc;
  //   int cbClsExtra;
  //   int cbWndExtra;
  //   HINSTANCE hInstance;
  //   HICON hIcon;
  //   HCURSOR hCursor;
  //   HBRUSH hbrBackground;
  //   LPCSTR lpszMenuName;
  //   LPCSTR lpszClassName;
  // } WNDCLASSA, *PWNDCLASSA, *NPWNDCLASSA, *LPWNDCLASSA;
  //
  windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
  windowClass.lpfnWndProc = MainWindowCallBack;

  // we can use getmoduleHandle(0) to get the instance
  windowClass.hInstance = Instance;
  // windowClass.hIcon = ;
  windowClass.lpszClassName = "HandmadeHero";

  // returns 0 if it errors
  if (RegisterClass(&windowClass)) {
    HWND Window = CreateWindowEx(
        0,                                // Optional window styles.
        windowClass.lpszClassName,        // Window class
        "HandmadeHero",                   // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,     // Parent window
        NULL,     // Menu
        Instance, // Instance handle
        NULL      // Additional application data
    );

    if (Window != NULL) {
      // start a message loop or windows will not send messsages to our window
      // windows put all messages that are trying to commune with us in a queue,
      // we need to start that loop and consuem
      // Correct.
      Running = true;
      int Xoff = 0;
      int Yoff = 0;
      while (Running) {

        // let's flush all the messages before we do anything else
        MSG msg;
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {

          if (msg.message == WM_QUIT) {
            Running = false;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
        for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
          XINPUT_STATE state;
          ZeroMemory(&state, sizeof(XINPUT_STATE));

          // Simply get the state of the controller from XInput.
          dwResult = XInputGetState(i, &state);

          if (dwResult == ERROR_SUCCESS) {
            // Controller is connected
            std::cout << "controller connected" << std::endl;
          } else {
            // Controller is not connected
            std::cout << "controller not connected" << std::endl;
          }
        }
        // done processing queue
        renderWeirdGradient(Xoff, Yoff);
        HDC DeviceContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int windowWidth = ClientRect.right - ClientRect.left;
        int windowHeight = ClientRect.bottom - ClientRect.top;

        win32UpdateWindow(DeviceContext, ClientRect, 0, 0, windowWidth,
                          windowHeight);
        ReleaseDC(Window, DeviceContext);

        Xoff += 1;
        // Yoff += 2;
        // get message loop blocks when there is no message and wait
        // we need to change to a peek message loop
        // the render before  to animate for now
        // if (GetMessage(&msg, NULL, 0, 0) > 0) {
        //
        //   TranslateMessage(&msg);
        //   DispatchMessage(&msg);
        // } else {
        //   break;
        // }
      }
    } else {
      // TODO(sk): Logging
    }
  } else {
    // TODO(sk): Logging
  }
  return 0;
};
