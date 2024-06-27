#ifndef WINDOW_H
#define WINDOW_H

struct Window
{
public:
  long x, y, width, height;

  Window(int width, int height) : x(0), y(0), width(width), height(height) {}
  Window(int xs, int ys, int xe, int ye) : x(xs), y(ys), width(xe - xs), height(ye - ys) {}
  Window(Window centerIn, int width, int height) : x(centerIn.x + centerIn.width / 2 - width / 2), y(centerIn.y + centerIn.height / 2 - height / 2), width(width), height(height) {}

  int xs() { return x; }
  int xe() { return x + width; }
  int ys() { return y; }
  int ye() { return y + height; }

  long size() { return width * height; }
};

#endif // WINDOW_H