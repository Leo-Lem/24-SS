#line 1 "/workspaces/24 SS/ES/exercises/.build/exercise/Window.h"

#ifndef WINDOW_H
#define WINDOW_H

struct Window
{
public:
  int x, y, width, height;

  Window(int width, int height) : x(0), y(0), width(width), height(height) {}
  Window(int xs, int xe, int ys, int ye) : x(xs), y(ys), width(xe - xs), height(ye - ys) {}

  int xs() { return x; }
  int xe() { return x + width; }
  int ys() { return y; }
  int ye() { return y + height; }

  int size() { return width * height; }
};

#endif // WINDOW_H