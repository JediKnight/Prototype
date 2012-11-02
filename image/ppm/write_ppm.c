#include <stdio.h>
#include <errno.h>

#define IMG_Y 640
#define IMG_X 640
#define COLOR 3
enum { R =0, G, B };

int main()
{
  FILE *fp;
  unsigned char image[IMG_Y][IMG_X][COLOR];
  int x, y, c = 0xff;

  if((fp = fopen("sample.pnm", "wb")) == NULL)
    {
      perror("fopen");
      return -1;
    }

  for(y = 0; y < IMG_Y; y++)
    {
      c = 0x01;
      for(x = 0; x < IMG_X; x++)
	{
	  image[y][x][R] = image[y][x][G] = image[y][x][B] = c;
	  if(y == x)
	    {
	      c = 0xff;
	      image[y][x][R] = image[y][x][G] = image[y][x][B] = 255;
	    }
	}
    }

  fprintf(fp, "P6\n%d %d\n%d\n", IMG_X, IMG_Y, 0xff);
  fwrite(image, sizeof(char), IMG_Y * IMG_X * COLOR, fp);
  fclose(fp);
  return 0;
}
