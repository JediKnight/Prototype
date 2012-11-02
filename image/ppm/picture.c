#include <math.h>

typedef struct tagImageData {
  double x;
  double y;
  double cx;
  double cy;
  double angle;
} ImageData;

int rotationMatrix(double x, double y, double angle, double cx, double cy)
{
  x1 = ((x - cx) * cos(angle)) + ((y - cy) * -sin(angle)) + (1 * 0) + cx;
  y1 = ((x - cx) * sin(angle)) + ((y - cy) * cos(angle)) + (i * 0) + cy;
  z1 = ((x - cx) * 0) + ((y -cy) * 0) + (1 * 1);
  return 0;
}

int scaringMatrix(double x, double y, double i, double bai)
{
  x1 = bai * x + 0 + 0;
  y1 = 0 + bai * y + 0;
  z1 = 0 + 0 + bai * 1;
  return 0;
}

int translationMatrix(double x, double y, double i)
{
  return 0;
}
