#include <boost/gil/gil_all.hpp>
#include <boost/gil/extension/io/jpeg_dynamic_io.hpp>
#include <iostream>


int main(int argc, char *argv[]) {

  boost::gil::rgb8_image_t img;
  jpeg_read_image("mona_lisa.jpg", img);
  std::cout << "Width: " << img.width() << std::endl;
  std::cout << "Height: " << img.height() << std::endl;

  auto func = [] () { std::cout << "Pixel\n" << std::endl; };
  boost::gil::for_each_pixel(const_view(img), func)
  //boost::gil::rgb8_pixel_t black(0, 0, 0);
  //fill_pixels(subimage_view(view(img), 10, 10, 100, 20), black);
  //jpeg_write_view("out.jpg", const_view(img));
  return 0;
}
