#include "handmade.h"
// #include <stdint.h>
// #include <math.h>

// #define internal static
// #define local_persist static
// #define global_var static

// typedef int32_t int32;
// typedef uint8_t uint8;
// typedef uint32_t uint32;

// typedef int16_t int16;
// typedef int32 bool32;

// typedef float real32;
// typedef double real64;





internal void gameOutPutSound(game_output_sound_buffer *SoundBuffer){

   local_persist real32 tSine;
   int ToneVolume = 3000;
   int ToneHz = 256;
   int WavePeriod = SoundBuffer->SamplesPersecond/ToneHz;
  int16 *SampleOut = SoundBuffer->Samples;
 for(int SampleIndex = 0; SampleIndex < SoundBuffer->SampleCount; ++SampleIndex){
       
          //  real32 t = sinf(tSine);
           real32 SineValue = sinf(tSine);
           int16 SampleValue =  (int16)(SineValue * ToneVolume);
           *SampleOut++ = SampleValue;
           *SampleOut++ = SampleValue;
       
           tSine += 2.0f*3.14159265357f*1.0f/(real32)WavePeriod;
        }

}


internal void renderWeirdGradient(game_offscreen_buffer *buffer,  int Xoffset, int Yoffset) {

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


internal void GameUpdateAndRender(game_offscreen_buffer *buffer, int BlueOffset, int GreenOffset, game_output_sound_buffer *SoundBuffer){

    gameOutPutSound(SoundBuffer);
    renderWeirdGradient(buffer, BlueOffset, GreenOffset);
    
};