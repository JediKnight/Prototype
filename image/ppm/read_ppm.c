#include <stdio.h>
#include <errno.h>

#define IMG_Y 640
#define IMG_X 640
#define RGB 3
#define getpos(y, x) (y * IMG_X) + x

int main()
{
  FILE *fp;
  unsigned char image[IMG_Y][IMG_X][RGB], tmpR, tmpG, tmpB;
  int x, y, i, tail;

  if((fp = fopen("sample.pnm", "rb")) == NULL)
    {
      perror("fopen");
      return -1;
    }

  fscanf(fp, "P6\n640 640\n255\n");
  puts("set image data");

  fread(image, sizeof(char), IMG_Y * IMG_X * RGB, fp);
  puts("read!!");
  fclose(fp);

  for(y = 0; y < IMG_Y; y++)
    {
      for(x = 0, tail = (y + 1) + IMG_X; x < IMG_X / 2; x++, --tail)
	{
	  tmpR = image[y][x][0];
	  tmpG = image[y][x][1];
	  tmpB = image[y][x][2];

	  printf("%d, %d, %d\n", tmpR, tmpG, tmpB);

	  image[y][x][0] = image[y][tail][0];
	  image[y][x][1] = image[y][tail][1];
	  image[y][x][1] = image[y][tail][2];

	  image[y][tail][0] = tmpR;
	  image[y][tail][1] = tmpG;
	  image[y][tail][2] = tmpB;
	}
    }

  if((fp = fopen("sample_rev.pnm", "wb")) == NULL)
    {
      perror("fopen");
      return -1;
    }
  
  fprintf(fp, "P6\n%d %d\n%d\n", IMG_X, IMG_Y, 0xff);
  fwrite(image, sizeof(image), IMG_Y * IMG_X * RGB, fp);
  puts("write!!");
  fclose(fp);
  return 0;
}
