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

char get_ascii_from_greyscale(int greyscale, const std::string& asciiPallette) {
  int index = greyscale/255.*asciiPallette.size();
  return asciiPallette[index];
}

int main(int argc, char *argv[])
{
  // Constructing options describing variables and giving it a textual
  // description *All options*
  opt::options_description desc("All options");

  // Adding options (a and o are shorthands for apples/oranges)
  desc.add_options()
    ("file,f", opt::value<std::string>(), "The input file path in jpeg format.")
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
    return 1;
  }

  // Setup some constants
  //std::string asciiPallette = "@#£=+|:. " ;
  std::string asciiPallette = "@O=-*,. " ;
  std::string inputFilePath;
  std::string outputFilePath = "output.txt";
  std::vector<unsigned char> greysMap;
  int resolution = 1;

  // Checking for options
  opt::notify(vm);
  if (vm.count("file")) {
    inputFilePath = vm["file"].as<std::string>();
  } else {
    std::cout << "Usage: jpg2ascii -f {filename}" << std::endl;
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
      asciiPallette = vm["palette"].as<std::string>();
    }
  }

  std::ofstream ofs(outputFilePath, std::ofstream::out);

  // Read in the file
  std::cout << "Reading in " << inputFilePath << std::endl;
  cimg_library::CImg<unsigned char> src(inputFilePath.c_str());

  // Populate the greyscale map
  std::cout << "Populating the greyscale map." << std::endl;
  for (int i = 0; i < src.height(); i+=resolution) {
    for (int j = 0; j < src.width(); j+=resolution) {
      int grey = src(j,i,0,0)*RED_COEFF + src(j,i,0,1)*GREEN_COEFF + src(j,i,0,2)*BLUE_COEFF;
      greysMap.push_back(grey);
    }
  }

  // Stream valid ascii chars to file
  std::cout << "Streaming ascii characters to output file" << std::endl;
  int lineWidth = src.width()/resolution;
  for (int i = 0; i < greysMap.size(); ++i) {
    ofs << get_ascii_from_greyscale(greysMap[i], asciiPallette);
    if ((i+1)%lineWidth == 0) {
      ofs << std::endl;
    }
  }
  ofs.close();
  std::cout << "Done! You can view the art at:" << std::endl;
  std::cout << outputFilePath << std::endl;
  return 0;
}
