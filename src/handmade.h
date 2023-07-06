#if !defined (HANDMADE_H)

#define internal static


struct  game_offscreen_buffer {

  void *Memory;
  int Width;
  int Height;
  int Picth;
};

// Timing, input, render buffer, sound buffer to output to
internal void GameUpdateAndRender(game_offscreen_buffer *buffer, int xOffest, int Yoffset);





#define HANDMADE_H
#endif