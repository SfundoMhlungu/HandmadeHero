
#include <iostream>
// #include <mmsystem.h>
#include <dsound.h>
#include <stdint.h>
#include <windows.h>
#include <xinput.h>
#include <math.h>
#include <malloc.h>
// #pragma comment(lib, "winmm.lib")




#define internal static
#define local_persist static
#define global_var static
#define PI32 3.14159265357f;

typedef int32_t int32;
typedef uint8_t uint8;
typedef uint32_t uint32;

typedef int16_t int16;
typedef int32 bool32;

typedef float real32;
typedef double real64;

#include "handmade.cpp"
struct  win32_offscreen_buffer {
  BITMAPINFO Info;
  void *Memory;
  int Width;
  int Height;
  int Picth; 
  int BytesPerPixel;

};

struct win32_sound_output {
        //  int TonesPerSecondHz;
      uint32 RunningSampleIndex;
      // int WavePeriod;
      // int HalfWavePeriod;
      int BytesPerSample;
      // int16 ToneVolume;
       int SamplesPersecond;
         int SecondaryBufferSize;
           int LatencySampleCount;
      };


global_var bool Running;
global_var bool isSoundPlaying;
global_var BITMAPINFO Bitmapinfo;
global_var void *BitmapMemory;
global_var int bitMapwidth;
global_var int bitMapheight;
global_var int BytesPerPixel = 4;
global_var LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;
global_var win32_offscreen_buffer Globalbuffer;




 internal debug_file_result DEBUGPlatformReadEntireFile(const char *Filename){
    void *Result = 0;
     debug_file_result fileResult = {};
  // Locate it in the physical so we can talk about it, I'll tell ya what to do withit
    HANDLE FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,0, 0);

    if(FileHandle != INVALID_HANDLE_VALUE){
           LARGE_INTEGER FileSize;
           if(GetFileSizeEx(FileHandle, &FileSize)){
               uint32 FileSize32 = safeTruncate64(FileSize.QuadPart);
               // WE NORMALLY USE HEAP ALLOC FOR FILES, virtual is for large things
                fileResult.ContentSize = FileSize32;
               Result = VirtualAlloc(0, FileSize32, MEM_COMMIT, PAGE_READWRITE);
               if(Result){
                    DWORD BytesRead;
                  if(ReadFile(FileHandle, Result,FileSize32,&BytesRead, 0) && (FileSize32 == BytesRead)){

                  }else{
                     DEBUGPlatformFreeFileMemory(Result);
                     Result = 0;
                  }
               }else{

               }
           }


           CloseHandle(FileHandle);
    }else{
        std::cout << "FAILED TO OPEN FILE" << std::endl;
    }
   
    fileResult.Contents = Result;
    return (fileResult);
 };
 internal void DEBUGPlatformFreeFileMemory(void *Memory){
    if(Memory){
      VirtualFree(Memory, 0, MEM_RELEASE);
    }

 };

  internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32_t MemorySize, void *Memory){
      HANDLE FileHandle = CreateFileA(Filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS,0, 0);
       bool32 Result = false;
        if(FileHandle != INVALID_HANDLE_VALUE){
         
               // WE NORMALLY USE HEAP ALLOC FOR FILES, virtual is for large things
               
            
                  DWORD BytesWritten;
                  if(WriteFile(FileHandle, Memory,MemorySize,&BytesWritten, 0)){
                  Result = (BytesWritten == MemorySize);
                  }else{
                      std::cout << "FAILED TO WRITE FILE" << std::endl;
                  }

                CloseHandle(FileHandle);
           }else{
              std::cout << "FAILED TO CREATE  FILE HANDLE" << std::endl;
           }

      return (Result);
          
    }
    




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
      uint8 Blue = uint8(X + Xoffset);
      uint8 Green = uint8(Y + Yoffset);

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

    uint32 VKCode = (uint32)WParam;
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
    { Result = DefWindowProcA(window, message, WParam, LParam); }
    break;
  }
  return (Result);
};



internal void win32ClearSoundBuffer(win32_sound_output *SoundOutput){
        VOID *Region1;
         DWORD Region1Size;
    
         VOID *Region2; 
         DWORD Region2Size;
        if(SUCCEEDED(GlobalSecondaryBuffer->Lock(
         0, 
         SoundOutput->SecondaryBufferSize,
         &Region1,
         &Region1Size,
         &Region2,
         &Region2Size,
         0))){


               uint8 *DestSample = (uint8 *)Region1;
      
        
        //  DWORD Region1SampleCount = Region1Size/SoundOutput->BytesPerSample;
        //  DWORD Region2SampleCount = Region2Size/SoundOutput->BytesPerSample;
          for(DWORD BufferIndex = 0; BufferIndex < Region1Size; ++BufferIndex){
        
            //  real32 t = 2.0f*3.14159265357f*(real32)SoundOutput->RunningSampleIndex / (real32)SoundOutput->WavePeriod;
            //  real32 SineValue = sinf(t);
            //  int16 SampleValue =  (int16)(SineValue * SoundOutput->ToneVolume);
            *DestSample++ = 0;
          
            }

            DestSample = (uint8 *)Region2;
           for(DWORD BufferIndex = 0; BufferIndex < Region2Size; ++BufferIndex){
        
            //  real32 t = 2.0f*3.14159265357f*(real32)SoundOutput->RunningSampleIndex / (real32)SoundOutput->WavePeriod;
            //  real32 SineValue = sinf(t);
            //  int16 SampleValue =  (int16)(SineValue * SoundOutput->ToneVolume);
             *DestSample++ = 0;
          
            }
              GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
         }
}



internal void win32FillSoundBuffer(win32_sound_output *SoundOutput, DWORD ByteToLock, DWORD BytesTowrite, game_output_sound_buffer *SourceBuffer){
   // int16  int16      - single sample
         // [LEFT  RIGHT]
         VOID *Region1;
         DWORD Region1Size;
    
         VOID *Region2; 
         DWORD Region2Size;
        if(SUCCEEDED(GlobalSecondaryBuffer->Lock(
         ByteToLock, 
         BytesTowrite,
         &Region1,
         &Region1Size,
         &Region2,
         &Region2Size,
         0))){
         
        int16 *DestSample = (int16 *)Region1;
        int16 *SourceSample = SourceBuffer->Samples;
        
         DWORD Region1SampleSize = Region1Size/SoundOutput->BytesPerSample;
        for(DWORD SampleIndex = 0; SampleIndex < Region1SampleSize; ++SampleIndex){
       
          //  real32 t = 2.0f*3.14159265357f*(real32)SoundOutput->RunningSampleIndex / (real32)SoundOutput->WavePeriod;
          //  real32 SineValue = sinf(t);
          //  int16 SampleValue =  (int16)(SineValue * SoundOutput->ToneVolume);
           *DestSample++ = *SourceSample++;
           *DestSample++ = *SourceSample++;
           ++SoundOutput->RunningSampleIndex;
        
        }
        DWORD Region2SampleSize = Region2Size/SoundOutput->BytesPerSample;
          DestSample = (int16 *)Region2;
        for(DWORD SampleIndex = 0; SampleIndex < Region2SampleSize; ++SampleIndex){
            
    
           *DestSample++ = *SourceSample++;
           *DestSample++ = *SourceSample++;
           ++SoundOutput->RunningSampleIndex;
        }

           GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
        }
}



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
      isSoundPlaying = false;
     
      int Xoff = 0;
      int Yoff = 0;
      win32_sound_output  SoundOutput = {};
      // SoundOutput.TonesPerSecondHz = 256;
      SoundOutput.RunningSampleIndex =0;
      SoundOutput.SamplesPersecond = 48000;
      // SoundOutput.WavePeriod =  SoundOutput.SamplesPersecond/SoundOutput.TonesPerSecondHz;
      // SoundOutput.HalfWavePeriod = SoundOutput.WavePeriod/2;
      SoundOutput.BytesPerSample =sizeof(int16)*2;
      // SoundOutput.ToneVolume = 5000;
      SoundOutput.LatencySampleCount = SoundOutput.SamplesPersecond /15;
     
       SoundOutput.SecondaryBufferSize = SoundOutput.SamplesPersecond *SoundOutput.BytesPerSample;
      
    
    
      // START OF SOUND STAFF
       win32initDsound(Window, SoundOutput.SamplesPersecond, SoundOutput.SecondaryBufferSize);
      //  win32FillSoundBuffer(&SoundOutput, 0, SoundOutput.SecondaryBufferSize, &);
      win32ClearSoundBuffer(&SoundOutput);
      GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
  #if 1
      int16 *Samples = (int16 *)VirtualAlloc(0, SoundOutput.SecondaryBufferSize, MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
  #else
      int16 *Samples = (int16 *)alloca(SoundOutput.SecondaryBufferSize);

  #endif
      game_memory Memory = {};
      Memory.PermanentStorageSize = Megabytes(64);
      Memory.PermanentStorage = VirtualAlloc(0,  Memory.PermanentStorageSize , MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);

      Memory.TransientStorageSize = Gigabytes(uint64_t(2));
      Memory.TransientStorage = VirtualAlloc(0,  Memory.TransientStorageSize , MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);

      //TODO(sk) check if samples and game memory were assigned correctly with an if, if not do not play the game
      if(Samples && Memory.PermanentStorage && Memory.TransientStorage){
        std::cout << "Mem allocated" << std::endl;
      }else{
         std::cout << "Mem allocated failed" << std::endl;
        Running = false;
        return 1;
      }
      while (Running) {

        // let's flush all the messages before we do anything else
        MSG msg;
        game_input Input = {};
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {

          if (msg.message == WM_QUIT) {
            Running = false;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
        // TODO(DAY 13): 44 minutes  to 105
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
           DWORD ByteToLock;
            DWORD PlayCursorPosition;
           DWORD WriteCursorPosition;
           DWORD BytesTowrite;
           DWORD TargetCursor;
           bool32 SoundIsValid = false;
         if(SUCCEEDED(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursorPosition, &WriteCursorPosition))){
    
     
         
          ByteToLock = ((SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) %  SoundOutput.SecondaryBufferSize);
          
          TargetCursor = ((PlayCursorPosition + (SoundOutput.LatencySampleCount*SoundOutput.BytesPerSample))% SoundOutput.SecondaryBufferSize);
          if(ByteToLock > TargetCursor){
           BytesTowrite = SoundOutput.SecondaryBufferSize - ByteToLock;
           BytesTowrite += TargetCursor;
         }else{
           BytesTowrite = TargetCursor - ByteToLock;
         }
    
        SoundIsValid = true;
     
      
     };
      //  renderWeirdGradient(&Globalbuffer,Xoff, Yoff);
       
         game_output_sound_buffer SoundBuffer = {};
         SoundBuffer.SamplesPersecond = SoundOutput.SamplesPersecond;
         // 30 frames
         SoundBuffer.SampleCount = BytesTowrite / SoundOutput.BytesPerSample;
         SoundBuffer.Samples = Samples;

         game_offscreen_buffer Buffer;
         Buffer.Memory = Globalbuffer.Memory;
         Buffer.Height = Globalbuffer.Height;
         Buffer.Width = Globalbuffer.Width;
         Buffer.Picth = Globalbuffer.Picth;
         GameUpdateAndRender(&Memory, &Input, &Buffer, &SoundBuffer);
       
    //   TODO SOUND START
   
     if(SoundIsValid){
    
     
         
        //  DWORD ByteToLock = (SoundOutput.RunningSampleIndex*SoundOutput.BytesPerSample) %  SoundOutput.SecondaryBufferSize;
        //  DWORD BytesTowrite;
        //  if(ByteToLock == PlayCursorPosition){
        //    BytesTowrite = 0;
        //  }else if(ByteToLock > PlayCursorPosition){
        //    BytesTowrite = SoundOutput.SecondaryBufferSize - ByteToLock;
        //    BytesTowrite += PlayCursorPosition;
        //  }else{
        //    BytesTowrite = PlayCursorPosition - ByteToLock;
        //  }
    
       win32FillSoundBuffer(&SoundOutput, ByteToLock, BytesTowrite, &SoundBuffer);
      
     };

      // if(!isSoundPlaying){
      //     GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
      //     isSoundPlaying = true;
      // }
    // TODO  SOUND END
      HDC DeviceContext = GetDC(Window);
        RECT ClientRect;
        GetClientRect(Window, &ClientRect);
        int windowWidth = ClientRect.right - ClientRect.left;
        int windowHeight = ClientRect.bottom - ClientRect.top;

        win32UpdateWindow(&Globalbuffer,DeviceContext, ClientRect, 0, 0, windowWidth,
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
