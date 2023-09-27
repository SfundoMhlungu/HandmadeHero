#if !defined (HANDMADE_H)
#include <stdint.h>
#define internal static

#define Assert(Expression) \
  if(!(Expression)) {*(int *)0=0;} 
#define ArrayCount(Array) (sizeof(Array))/ sizeof((Array)[0])
#define Kilobytes(Value) (Value * 1024 )
#define Megabytes(Value) (Kilobytes(Value)*1024)
#define Gigabytes(Value) (Megabytes(Value)*1024)

typedef int16_t int16;
typedef int32_t int32;
typedef int32 bool32;
typedef float real32;
typedef double real64;



#if 1 
struct debug_file_result{
void *  Contents;
uint32_t ContentSize;
};
 internal debug_file_result DEBUGPlatformReadEntireFile(const char *Filename);
 internal void DEBUGPlatformFreeFileMemory(void *Memory);

  internal bool32 DEBUGPlatformWriteEntireFile(char *Filename, uint32_t MemorySize, void *Memory);

#else
#endif 


inline uint32 safeTruncate64(uint64_t Value){
  if(Value > 0xFFFFFFFF){
    // TODO() exit
  }
  uint32 Result = (uint32)Value;

  return (Result);
}

struct  game_offscreen_buffer {

  void *Memory;
  int Width;
  int Height;
  int Picth;
};



struct game_output_sound_buffer{
   int SamplesPersecond;
   int SampleCount;
   int16 *Samples;
};



struct game_button_state {
  int HalfTransition;
  bool32 EndedDown;
};

struct game_controller_input {
  bool32 IsAnalog;

  real32  StartX;
  real32 StartY;

  real32 MinX;
  real32 MinY;

  real32 MaxX;
  real32 MaxY;

  real32 EndX;
  real32 EndY;
    union{
      game_button_state Buttons[6];
      struct{
        game_button_state Up;
        game_button_state Down;
        game_button_state Left;
        game_button_state Right;
        game_button_state LeftShoulder;
        game_button_state RightShoulder;
      };
    };
};

struct game_input {
  game_controller_input Controllers[4];
};


struct game_memory {
    bool32 isInitialized;
    uint64_t PermanentStorageSize;
    void *PermanentStorage;
     uint64_t TransientStorageSize;
    void *TransientStorage;
};
// Timing, input, render buffer, sound buffer to output to
internal void GameUpdateAndRender(game_memory *Memory,game_input *Input, game_offscreen_buffer *buffer, game_output_sound_buffer *SoundBuffer);


struct game_state {
    int BlueOffset; 
     int GreenOffset; 
     int ToneHz ;
};




#define HANDMADE_H
#endif