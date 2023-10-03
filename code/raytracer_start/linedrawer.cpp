/***************************************************************************
 *
 * krt - Kens Raytracer - Coursework Edition. (C) Copyright 1997-2019.
 *
 * Do what you like with this code as long as you retain this comment.
 */

/* This is the code you will need to replace for Lab 1.
 *
 * It contains two simple implementations that loop over the longest axis adding the gradient to the position on the other axis at each step.
 * The objective is for you to remove the use of floating point variables and work entirely with integers.
 * You should use Bresenhams Line Algorithm to achieve this.
 *
 * Note this code does not do clipping. Keep the object inside the viewport.
 */

#include <iostream>
#include "linedrawer.h"

int draw_x_line(FrameBuffer *fb, int sx, int sy, int ex, int ey)
{
  int dir = 1;
  if (sx > ex)
  {
    dir = -1;
  }

  int x = sx;
  int wy = sy;
  int dy = ey - sy;
  int dx = ex - sx;
  int fy = dy / 2;
  
  while (x != ex)
  {
    fb->plotPixel(x, wy, 1.0f, 1.0f, 1.0f);

    fy += dy * dir;

    // Negative slope
    if (dy * dir < 0){
      if (fy <= -dx){
        wy -= dir;
        fy += dx * dir;
      }
    }
    // Positive slope
    else if (dy * dir > 0){
      if (fy >= dx){
      wy += dir;
      fy -= dx * dir;
      }
    }

    // Zero slope

    x += dir;
  }

  return 0;
}

int draw_y_line(FrameBuffer *fb, int sx, int sy, int ex, int ey)
{
  int dir = 1;
  if (sy > ey)
  {
    dir = -1;
  }

  int wx = sx;
  int y = sy;
  int dy = ey - sy;
  int dx = ex - sx;
  int fx = dx / 2;
  
  while (y != ey)
  {
    fb->plotPixel(wx, y, 1.0f, 1.0f, 1.0f);

    fx += dx * dir;

    // Negative slope
    if (dx * dir < 0){
      if (fx <= -dy){
        wx -= dir;
        fx += dy * dir;
      }
    }
    // Positive slope
    else if (dx * dir > 0){
      if (fx >= dy){
      wx += dir;
      fx -= dy * dir;
      }
    }

    // Zero slope

    y += dir;
  }

  return 0;
}


int draw_line(FrameBuffer *fb, int sx, int sy, int ex, int ey)
{
  if ((sx == ex) && (sy==ey))
  {
    return fb->plotPixel(sx, sy, 1.0f, 1.0f, 1.0f);
    
  } else if (((ex-sx)* (ex-sx)) >= ((ey-sy)* (ey-sy)))
  {
    return draw_x_line(fb, sx, sy, ex, ey);
    
  } else
  {
    return draw_y_line(fb, sx, sy, ex, ey);
  }
}
