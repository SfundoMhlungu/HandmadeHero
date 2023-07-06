#include "handmade.h"


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


internal void GameUpdateAndRender(game_offscreen_buffer *buffer, int BlueOffset, int GreenOffset){


    renderWeirdGradient(buffer, BlueOffset, GreenOffset);
    
};