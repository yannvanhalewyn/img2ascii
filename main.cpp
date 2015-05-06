#include <CImg.h>
#include <iostream>
#include <fstream>


// These are constants used to transform rbg-data into greyscale
#define RED_COEFF 0.2125
#define GREEN_COEFF 0.7154
#define BLUE_COEFF 0.0721

void to_greyscale(cimg_library::CImg<unsigned char>& src) {
  for (int i = 0; i < src.width(); i++) {
    for (int j = 0; j < src.height(); j++) {
      int grey = src(i,j,0,0)*RED_COEFF + src(i,j,0,1)*GREEN_COEFF + src(i,j,0,2)*BLUE_COEFF;
      src(i,j,0,0) = grey;
      src(i,j,0,1) = grey;
      src(i,j,0,2) = grey;
    }
  }
}

char get_ascii_from_greyscale(int greyscale, const std::string& among) {
  int index = greyscale/255.*among.size();
  return among[index];
}

int main(int argc, char *argv[])
{
  cimg_library::CImg<unsigned char> src("test_imgs/mona_lisa.jpg");

  std::string among = "@#£=+|:. " ;
  std::ofstream ofs("output.txt", std::ofstream::out);
  //to_greyscale(src);

  for (int i = 0; i < src.width(); i++) {
    for (int j = src.height()-1; j >= 0; j--) {
      ofs << get_ascii_from_greyscale(src(i,j,0,0), among);
    }
    ofs << "\n";
  }
  ofs.close();
  //cimg_library::CImgDisplay main_disp(src, "monalisa");
  //while(!main_disp.is_closed()) {
    //main_disp.wait();
  //}
  return 0;
}

