#if !defined (HANDMADE_H)
// #include <stdint.h>
// #define internal static
// typedef int16_t int16;


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
// Timing, input, render buffer, sound buffer to output to
internal void GameUpdateAndRender(game_offscreen_buffer *buffer, int xOffest, int Yoffset, game_output_sound_buffer *SoundBuffer);





#define HANDMADE_H
#endif