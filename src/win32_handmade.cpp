
#include <iostream>
// #include <mmsystem.h>
#include <dsound.h>
#include <stdint.h>
#include <windows.h>
#include <xinput.h>
// #pragma comment(lib, "winmm.lib")

#define internal static
#define local_persist static
#define global_var static
typedef int32_t int32;
typedef uint8_t uint8;
typedef uint32_t uint32;

typedef int16_t int16;
typedef int32 bool32;


struct  win32_offscreen_buffer {
  BITMAPINFO Info;
  void *Memory;
  int Width;
  int Height;
  int Picth; 
  int BytesPerPixel;
};

global_var bool Running;
global_var BITMAPINFO Bitmapinfo;
global_var void *BitmapMemory;
global_var int bitMapwidth;
global_var int bitMapheight;
global_var int BytesPerPixel = 4;
global_var LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;
global_var win32_offscreen_buffer Globalbuffer;

#define DIRECT_SOUND_CREATE(name)                                              \
  HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDs,              \
                      LPUNKNOWN pUnkOuter);

typedef DIRECT_SOUND_CREATE(direct_sound_create);
internal void win32initDsound(HWND Window, int32 SamplesPersecond = 48000,
                              int32 BufferSize = 48000) {

  // TODO(sk): load direct sound library
  HMODULE DSoundLibrary = LoadLibraryA("dsound.dll");

  if (DSoundLibrary) {
    // TODO(sk): get direct sound object
    direct_sound_create *DirectSoundCreate =
        (direct_sound_create *)GetProcAddress(DSoundLibrary,
                                              "DirectSoundCreate");

    LPDIRECTSOUND DirectSound;

    if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0))) {
      if (SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))) {
        DSBUFFERDESC BufferDescription;
        BufferDescription.dwSize = sizeof(BufferDescription);
        BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

        LPDIRECTSOUNDBUFFER PrimaryBuffer;
        WAVEFORMATEX WaveFormat = {};
        WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
        WaveFormat.nChannels = 2;
        WaveFormat.nSamplesPerSec = SamplesPersecond;
        WaveFormat.wBitsPerSample = 16;

        // LEFT RIGHT LEFT RIGHT (CHANNELS) nBlockAlign =  1 [left right]
        WaveFormat.nBlockAlign =
            (WaveFormat.nChannels * WaveFormat.wBitsPerSample) / 8;
        WaveFormat.nAvgBytesPerSec =
            WaveFormat.nSamplesPerSec * WaveFormat.nBlockAlign;
        WaveFormat.cbSize = 0;

        if (SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription,
                                                     &PrimaryBuffer, 0))) {
          if (SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))) {
            // STOPPED AT 52 MINUTES CREATING SECONDARY BUFFER
              std::cout << "primary buffer created" << std::endl;
          } else {
            std::cout << "primary buffer failed" << std::endl;
          }
        }

         BufferDescription.dwFlags = 0;
         BufferDescription.dwBufferBytes = BufferSize;
         BufferDescription.lpwfxFormat = &WaveFormat;

                if(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0))){
                      std::cout << "Secondary buffer created" << std::endl;
        }else{
           std::cout << "Secondary buffer failed" << std::endl;
        }

      } else {
            //TODO(sk): Logging,DirectSoundCreate
            std::cout << "Failed to call DirectSoundCreate" << std::endl;

      }
      // TODO(sk): "create" primary buffer(set mode)

      // TODO(sk): "create" secondary buffer
      // BufferDescription.dwSize = BufferSize;
      // TODO(sk): start playing sound
    }
  }
}

internal void renderWeirdGradient(win32_offscreen_buffer *buffer,  int Xoffset, int Yoffset) {

  int Width = buffer->Width;
  int Height = buffer->Height;
  
  uint8 *Row = (uint8 *)buffer->Memory;
  for (int Y = 0; Y < Height; ++Y) {
    uint32 *Pixel = (uint32 *)Row;
    for (int X = 0; X < Width; ++X) {
      // RGB IS BGRXX in memory day 4, 38 minutes explanation
      // iN MEMORY pixel : 00 00 00 00
      // BB GG RR XX

      // BLUE
      uint8 Blue = (X + Xoffset);
      uint8 Green = (Y + Yoffset);

      *Pixel++ = ((Green << 8) | Blue);
   
    }

    Row += buffer->Picth;
  }
}
internal void win32ResizeDIBSection(win32_offscreen_buffer *buffer,int width, int height) {
  // TODO(sk): maybe delete after new creation
  if (buffer->Memory) {
    // TODO(): virtual protect, make sure this space is not used, allowing code
    // to touch etc
    VirtualFree(buffer->Memory, 0, MEM_RELEASE);
  }

  buffer->Width = width;
  buffer->Height = height;
  buffer->BytesPerPixel = 4;
  buffer->Info.bmiHeader.biSize = sizeof(buffer->Info.bmiHeader);

  buffer->Info.bmiHeader.biWidth = buffer->Width;
  // positive height, the bit "draw" starts bottom left
  // negative top right(may have perfomance issues?)
  buffer->Info.bmiHeader.biHeight = -buffer->Height;
  buffer->Info.bmiHeader.biPlanes = 1;

  // 8 bit for r, 8 for b, 8 for gree
   buffer->Info.bmiHeader.biBitCount = 32;
  // no compression, to write to it fatser
  buffer->Info.bmiHeader.biCompression = BI_RGB;
  // no need static global will make zero for us
  // Bitmapinfo.bmiHeader.biSizeImage = 0;
  // Bitmapinfo.bmiHeader.biXPelsPerMeter = 0;
  // Bitmapinfo.bmiHeader.biYPelsPerMeter = 0;
  // Bitmapinfo.bmiHeader.biClrUsed = 0;
  // Bitmapinfo.bmiHeader.biClrImportant = 0

    int BitmapMemorySize = (buffer->Width * buffer->Height) * buffer->BytesPerPixel;
   buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
   buffer->Picth = width * buffer->BytesPerPixel;
  // renderWeirdGradient(0, 0);
};
internal void win32UpdateWindow(win32_offscreen_buffer *buffer, HDC DeviceContext, RECT ClientRect, int x,
                                int y, int width, int height) {

  int windowWidth = ClientRect.right - ClientRect.left;
  int windowHeight = ClientRect.bottom - ClientRect.top;
  //  renderWeirdGradient(x, y);
  StretchDIBits(DeviceContext, 0, 0, buffer->Width, buffer->Height, 0, 0,
                windowWidth, windowHeight, buffer->Memory, &buffer->Info,
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
    win32ResizeDIBSection(&Globalbuffer, Width, Height);
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

  case WM_SYSKEYDOWN:
  case WM_SYSKEYUP:
  case WM_KEYDOWN:
  case WM_KEYUP: {
    // returns VIRTUAL KEYCODES, listing on mdn

    uint32 VKCode = WParam;
    // lparam tells the previous state of the key
    // the 30 bit (1 << 30) tells us if it was previously down(last frame)
    bool wasDown = ((LParam & (1 << 30)) != 0);
    // if keyup bit == 1
    bool isDown = ((LParam & (1 << 31)) == 0);
    if (VKCode == 'W') {

    } else if (VKCode == 'S') {
    } else if (VKCode == 'D') {
    } else if (VKCode == 'A') {
    } else if (VKCode == 'Q') {
    } else if (VKCode == 'E') {
    } else if (VKCode == VK_UP) {
    } else if (VKCode == VK_DOWN) {
    } else if (VKCode == VK_LEFT) {
    } else if (VKCode == VK_RIGHT) {
    } else if (VKCode == VK_SPACE) {
      if (isDown && !wasDown) {

        std::cout << "SPAMING SPACE" << std::endl;
      }

      if (isDown && wasDown) {

        std::cout << "HOLDING SPACE" << std::endl;
      }
    } else if (VKCode == VK_ESCAPE) {
    }

    bool32 AltKeydown = (LParam & (1 << 29));
    if (VKCode == VK_F4 && AltKeydown) {

      Running = false;
    }
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

    win32UpdateWindow(&Globalbuffer,DeviceContext, ClientRect_, x, y, Width, Height);
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

  WNDCLASSA windowClass = {};
  
  windowClass.style = CS_HREDRAW | CS_VREDRAW;
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
      int SamplesPersecond = 48000;
      int Xoff = 0;
      int Yoff = 0;
      int TonesPerSecondHz = 256;
      
      uint32 RunningSampleIndex =0;
      int SquareWavePeriod =  SamplesPersecond/TonesPerSecondHz;
      int HalfSquareWavePeriod = SquareWavePeriod/2;
      int BytesPerSample =sizeof(int16)*2;
      int16 ToneVolume = 6000;
      int SecondaryBufferSize = SamplesPersecond *BytesPerSample;
  //    win32initDsound(Window, SamplesPersecond, SecondaryBufferSize);
//      GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
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
        // for (DWORD i = 0; i < XUSER_MAX_COUNT; i++) {
        //   XINPUT_STATE state;
        //   ZeroMemory(&state, sizeof(XINPUT_STATE));

        //   // Simply get the state of the controller from XInput.
        //   dwResult = XInputGetState(i, &state);

        //   if (dwResult == ERROR_SUCCESS) {
        //     // Controller is connected
        //     // Day 006 from 13 to  minutes, I do not have an xbox controller,
        //     // ignore for now , also learnt dynamic linking or imports, just
        //     // start at 28 minutes
        //     std::cout << "controller connected" << std::endl;
        //   } else {
        //     // Controller is not connected
        //     //  std::cout << "controller not connected" << std::endl;
        //   }
        // }
        // done processing queue
       renderWeirdGradient(&Globalbuffer,Xoff, Yoff);
       
    //
    // DWORD PlayCursorPosition;
    // DWORD WriteCursorPosition;
    //  if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursorPosition, &WriteCursorPosition))){
    //
    //  
    //      
    //      DWORD ByteToLock = RunningSampleIndex*BytesPerSample %  SecondaryBufferSize;
    //      DWORD BytesTowrite;
    //      if(ByteToLock > PlayCursorPosition){
    //        BytesTowrite = SecondaryBufferSize - ByteToLock;
    //        BytesTowrite += PlayCursorPosition;
    //      }else{
    //        BytesTowrite = PlayCursorPosition - ByteToLock;
    //      }
    //
    //       // int16  int16      - single sample
    //      // [LEFT  RIGHT]
    //      VOID *Region1;
    //      DWORD Region1Size;
    //
    //      VOID *Region2; 
    //      DWORD Region2Size;
    //     if(SUCCEEDED(GlobalSecondaryBuffer->Lock(
    //      ByteToLock, 
    //      BytesTowrite,
    //      &Region1,
    //      &Region1Size,
    //      &Region2,
    //      &Region2Size,
    //      0))){
    //      
    //     int16 *SampleOut = (int16 *)Region1;
    //     
    //      DWORD Region1SampleCount = Region1Size/BytesPerSample;
    //      DWORD Region2SampleCount = Region2Size/BytesPerSample;
    //     for(DWORD SampleIndex = 0; SampleIndex < Region1SampleCount; ++SampleIndex){
    //    
    //
    //        int16 SampleValue =  ((RunningSampleIndex / HalfSquareWavePeriod)%2) ? ToneVolume : -ToneVolume;; 
    //        *SampleOut++ = SampleValue;
    //        *SampleOut++ = SampleValue;
    //        ++RunningSampleIndex;
    //     
    //     }
    //    SampleOut = (int16 *)Region2;
    //     for(DWORD SampleIndex = 0; SampleIndex < Region2SampleCount; ++SampleIndex){
    //         
    //
    //        int16 SampleValue =  ((RunningSampleIndex / HalfSquareWavePeriod)%2) ? ToneVolume : -ToneVolume;
    //        *SampleOut++ = SampleValue;
    //        *SampleOut++ = SampleValue;
    //         ++RunningSampleIndex;
    //     }
    //
    //     }
    //
    //  };
      HDC DeviceContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int windowWidth = ClientRect.right - ClientRect.left;
        int windowHeight = ClientRect.bottom - ClientRect.top;

        win32UpdateWindow(&Globalbuffer,DeviceContext, ClientRect, 0, 0, windowWidth,
                          windowHeight);
        ReleaseDC(Window, DeviceContext);

          Xoff += 1;
          Yoff += 2;
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
