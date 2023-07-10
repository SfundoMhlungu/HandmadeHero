#include "handmade.h"
#include <stdint.h>
#include <math.h>

#define internal static
#define local_persist static
#define global_var static

typedef int32_t int32;
typedef uint8_t uint8;
typedef uint32_t uint32;

typedef int16_t int16;
typedef int32 bool32;

typedef float real32;
typedef double real64;





internal void gameOutPutSound(game_output_sound_buffer *SoundBuffer, int ToneHz){

   local_persist real32 tSine;
   int ToneVolume = 3000;

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


internal void GameUpdateAndRender(game_memory *Memory, game_input *Input, game_offscreen_buffer *buffer, game_output_sound_buffer *SoundBuffer){
    // Assert(sizeof(game_state) <= sizeof(Memory->PermanentStorageSize));
    // TODO(debug mode) day 14, 45 minutes, 51 min combine transient and perm, for contigous mem from the same base address , 55 min correct code
    game_controller_input *Input0 = &Input->Controllers[0];
    game_state *GameState = (game_state *)Memory->PermanentStorage;

  
    if(!Memory->isInitialized){
        char *Filename = __FILE__;
       debug_file_result  fileResult =  DEBUGPlatformReadEntireFile(Filename);

        if(fileResult.Contents){
           //TODO(sk) SETUP DATA DIRECTORY(CALLED WORKING DIR) USING COMPILER FLAGS
           
          // std::cout << "FILE READ CLOSING NOW \n" << fileResult.Contents << fileResult.ContentSize << std::endl;
          // DEBUGPlatformWriteEntireFile("C:/Users/Sifundo-Mhlungu/Downloads/Video/random/systemsP/c++/build/test.out", fileResult.ContentSize, fileResult.Contents);
          DEBUGPlatformFreeFileMemory(fileResult.Contents);
        }


         GameState->BlueOffset = 0;
         GameState->GreenOffset = 0;
         GameState->ToneHz = 256;

         Memory->isInitialized = true;
    }
    if(Input0->IsAnalog){
       GameState->ToneHz = 256 + (int)(128.0f *(Input0->EndX));
      GameState->BlueOffset += (int)4.0f*(Input0->EndY);
    }else{

    }
    if(Input0->Down.EndedDown){
         GameState->GreenOffset += 1; 
    }
   
    gameOutPutSound(SoundBuffer, GameState->ToneHz);
    renderWeirdGradient(buffer, GameState->BlueOffset,  GameState->GreenOffset);
    
};