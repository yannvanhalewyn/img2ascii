#include <CImg.h>
#include <iostream>

int main(int argc, char *argv[])
{
  cimg_library::CImg<unsigned char> src("mona_lisa.jpg");
  int width = src.width();
  int height = src.height();

  cimg_library::CImgDisplay main_disp(src, "monalisa");
  while(!main_disp.is_closed()) {
    main_disp.wait();
  }

  std::cout << width << std::endl;
  std::cout << height << std::endl;
  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      std::cout << "(" << i << "," << j << ") ="
        << " R" << (int)src(i,j,0,0)
        << " G" << (int)src(i,j,0,1)
        << " B" << (int)src(i,j,0,2) << std::endl;
    }
  }
  return 0;
}

