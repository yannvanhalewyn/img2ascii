#include <CImg.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

namespace opt = boost::program_options;

// These are constants used to transform rbg-data into greyscale
#define RED_COEFF 0.2125
#define GREEN_COEFF 0.7154
#define BLUE_COEFF 0.0721

/*
 * This function parses the command line arguments using boost's program options
 * library, and returns a variables_map object.
 */
opt::variables_map parse_arguments(int argc, char *argv[]) {

  // Constructing options describing variables
  opt::options_description desc("All options");

  // Adding options (a and o are shorthands for apples/oranges)
  desc.add_options()
    ("file,f", opt::value<std::string>(), "The input file path in jpeg format. If a file is given, any passed in url's will be ignored.")
    ("url,u", opt::value<std::string>(), "A url hosting the image.")
    ("output,o", opt::value<std::string>(), "The output file path.")
    ("resolution,r", opt::value<int>(), "The chunk resulution. How many pixels will go in one output character. Cannot be a negative number.")
    ("palette", opt::value<std::string>(), "A custom string representing all ascii characters to be used in the output. Order the characters from 'dark' to 'light', e.g.: '@O=-*,. '. Cannot be empty.")
    ("help,h", "produce help message")
  ;

  // Variable to store our command line args
  opt::variables_map vm;

  // Parsing and storing args
  opt::store(opt::parse_command_line(argc, argv, desc), vm);
  opt::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(1);
  }
  return vm;
}

/*
 * This function takes in a CImg object, iterates through all the pixels and calculates
 * a greyscale luminous value for it. Those values are stored in a vector and are returned
 * to the caller
 */
std::vector<unsigned char> generate_greyscale_map(const cimg_library::CImg<unsigned char>& src, int resolution = 1) {
  std::cout << "Populating the greyscale map." << std::endl;
  std::vector<unsigned char> greysMap;
  for (int i = 0; i < src.height(); i+=resolution) {
    for (int j = 0; j < src.width(); j+=resolution) {
      int grey = src(j,i,0,0)*RED_COEFF + src(j,i,0,1)*GREEN_COEFF + src(j,i,0,2)*BLUE_COEFF;
      greysMap.push_back(grey);
    }
  }
  return greysMap;
}

/*
 * This function returns a char from the palette based on a greyscale value between 0 and 255.
 */
char get_ascii_from_greyscale(unsigned char greyscale, const std::string& asciiPalette) {
  int index = greyscale/255.*asciiPalette.size();
  return asciiPalette[index];
}

/*
 * This function writes takes care of streaming ascii art to the output file.
 * This is done based on a map of greyscale values and a width.
 */
void write_ascii_art(const std::vector<unsigned char>& greysMap, int lineWidth, std::string outputFilePath, std::string asciiPalette) {
  std::cout << "Streaming ascii characters to output file" << std::endl;
  std::ofstream ofs(outputFilePath, std::ofstream::out);
  for (int i = 0; i < greysMap.size(); i++) {
    ofs << get_ascii_from_greyscale(greysMap[i], asciiPalette);
    if ((i+1)%lineWidth == 0) {
      ofs << std::endl;
    }
  }
  ofs.close();
}

struct Options {
  //std::string asciiPallette = "@#£=+|:. ";
  std::string asciiPalette = "@O=-*,. " ;
  std::string inputFilePath;
  std::string outputFilePath = "output.txt";
  std::string url;
  int resolution = 1;
}

/*
 * =============
 * MAIN FUNCTION
 * =============
 */
int main(int argc, char *argv[])
{

  Options opts;

  // Parsing arguments
  opt::variables_map vm = parse_arguments(argc, argv);
  opt::notify(vm);
  if (vm.count("file")) {
    opts.inputFilePath = vm["file"].as<std::string>();
  } else if (vm.count("url")){
    opts.url = vm["url"].as<std::string>();
  } else {
    std::cout << "Usage: img2ascii -f {filename} OR img2ascii -u {url}" << std::endl;
    return 1;
  }
  if (vm.count("output")) outputFilePath = vm["output"].as<std::string>();
  if (vm.count("resolution")) {
    if (vm["resolution"].as<int>() <= 0) {
      std::cout << "Resolution has to be higher than 0. Using default." << std::endl;
    } else {
      resolution = vm["resolution"].as<int>();
    }
  }
  if (vm.count("palette")) {
    if (vm["palette"].as<std::string>().length() == 0) {
      std::cout << "Palette cannot be empty string. Using default." << std::endl;
    } else {
      asciiPalette = vm["palette"].as<std::string>();
    }
  }

  // Read in the file
  std::cout << "Reading in " << inputFilePath << std::endl;
  cimg_library::CImg<unsigned char> src(inputFilePath.c_str());

  // Populate the greyscale map
  std::vector<unsigned char> greysMap = generate_greyscale_map(src, resolution);

  // Stream valid ascii chars to file
  int lineWidth = src.width()/resolution;
  write_ascii_art(greysMap, lineWidth, outputFilePath, asciiPalette);
  std::cout << "Done! You can view the art at:" << std::endl;
  std::cout << outputFilePath << std::endl;
  return 0;
}
