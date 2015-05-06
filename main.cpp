#include <CImg.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>


// These are constants used to transform rbg-data into greyscale
#define RED_COEFF 0.2125
#define GREEN_COEFF 0.7154
#define BLUE_COEFF 0.0721

char get_ascii_from_greyscale(int greyscale, const std::string& asciiPallette) {
  int index = greyscale/255.*asciiPallette.size();
  return asciiPallette[index];
}

int main(int argc, char *argv[])
{
  // Setup some constants
  //std::string asciiPallette = "@#£=+|:. " ;
  std::string asciiPallette = "@O=-*,. " ;
  std::ofstream ofs("output.txt", std::ofstream::out);
  std::vector<unsigned char> greysMap;
  const int XRESOLUTION = 1;
  const int YRESOLUTION = XRESOLUTION; // In notepad, this looks better

  // Read in the file
  cimg_library::CImg<unsigned char> src("test_imgs/mona_lisa.jpg");

  // Populate the greyscale map
  for (int i = 0; i < src.height(); i+=YRESOLUTION) {
    for (int j = 0; j < src.width(); j+=XRESOLUTION) {
      int grey = src(j,i,0,0)*RED_COEFF + src(j,i,0,1)*GREEN_COEFF + src(j,i,0,2)*BLUE_COEFF;
      greysMap.push_back(grey);
    }
  }

  // Stream valid ascii chars to file
  int lineWidth = src.width()/XRESOLUTION;
  for (int i = 0; i < greysMap.size(); ++i) {
    ofs << get_ascii_from_greyscale(greysMap[i], asciiPallette);
    if ((i+1)%lineWidth == 0) {
      ofs << std::endl;
    }
  }
  //for (int i = 0; i < src.height(); i++) {
    //for (int j = 0; j < src.width(); j++) {
    //}
    //ofs << "\n";
  //}
  ofs.close();
  return 0;
}
