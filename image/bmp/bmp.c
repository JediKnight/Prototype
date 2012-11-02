#include <stdio.h>

int main()
{
  FILE *fp;
  unsigned char image[1024];
  fp = fopen("sample.bmp", "rb");
  fread(image, sizeof(image), 1024, fp);
  printf("%s\n", image);
  fclose(fp);
  return 0;
}
