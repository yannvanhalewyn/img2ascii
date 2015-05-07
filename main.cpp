#include <CImg.h>
#include <iostream>
#include <fstream>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/program_options/errors.hpp>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

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
    ("file,f", opt::value<std::string>(), "The input file path in jpeg format. \
If a file is given, any passed in url's will be ignored.")
    ("url,u", opt::value<std::string>(), "A url hosting the image.")
    ("facebook", opt::value<std::string>(), "A facebook username or profile ID. \
img2ascii will then download that person's most recent facebook profile picture.")
    ("output,o", opt::value<std::string>(), "The output file path.")
    ("resolution,r", opt::value<int>(), "The chunk resulution. How many pixels \
will go in one output character. Cannot be a negative number.")
    ("palette", opt::value<std::string>(), "A custom string representing all \
ascii characters to be used in the output. Order the characters from 'dark' \
to 'light', e.g.: '@O=-*,. '. Cannot be empty.")
    ("help,h", "produce help message")
  ;

  // Variable to store our command line args
  opt::variables_map vm;

  // Parsing and storing args
  opt::store(opt::parse_command_line(argc, argv, desc), vm);
  opt::notify(vm);
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  return vm;
}

/*
 * This function takes in a CImg object, iterates through all the pixels and
 * calculates a greyscale luminous value for it. Those values are stored in
 * a vector and are returned to the caller
 */
std::vector<unsigned char> generate_greyscale_map(const cimg_library::CImg<unsigned char>& src,
    int resolution = 1) {
  std::cout << "Populating the greyscale map - " <<
    "Width: " << src.width() << " | Height: " << src.height() <<std::endl;
  std::vector<unsigned char> greysMap;
  int required_memory = ceil((float)src.width()/resolution) * ceil((float)src.height()/resolution);
  greysMap.reserve(required_memory);
  for (int i = 0; i < src.height(); i+=resolution) {
    for (int j = 0; j < src.width(); j+=resolution) {
      int grey = src(j,i,0,0)*RED_COEFF + src(j,i,0,1)*GREEN_COEFF +
        src(j,i,0,2)*BLUE_COEFF;
      greysMap.push_back(grey);
    }
  }
  return greysMap;
}

/*
 * This function returns a char from the palette based on a greyscale value
 * between 0 and 255.
 */
char get_ascii_from_greyscale(unsigned char greyscale,
    const std::string& asciiPalette) {
  int index = greyscale/255.*asciiPalette.size();
  return asciiPalette[index];
}

/*
 * This function writes takes care of streaming ascii art to the output file.
 * This is done based on a map of greyscale values and a width.
 */
void write_ascii_art(const std::vector<unsigned char>& greysMap, int lineWidth,
    std::string outputFilePath, std::string asciiPalette) {
  std::cout << "Streaming ascii characters to output file" << std::endl;
  std::ofstream ofs(outputFilePath, std::ofstream::out);
  for (int i = 0; i < greysMap.size(); i++) {
    ofs << get_ascii_from_greyscale(greysMap[i], asciiPalette);
    if ((i+1)%lineWidth == 0) {
      ofs << std::endl;
    }
  }
  ofs.close();
  std::cout << "Written " << greysMap.size() << " characters to file!" << std::endl;
}

/*
 * The struct used for storing all user options
 */
struct Options {
  //std::string asciiPallette = "@#£=+|:. ";
  //std::string asciiPalette = "@O=-*,. " ;
  std::string asciiPalette = "@#8&o:*. " ;
  std::string inputFilePath;
  std::string outputFilePath = "output.txt";
  std::string url;
  int resolution = 1;
};


/*
 * This method reads the variables_map object and stores that information
 * in a Option data-structure which is returned
 */
Options parse_variable_map(opt::variables_map& vm) {
  Options opts;

  vm.notify();
  if (vm.count("file")) {
    opts.inputFilePath = vm["file"].as<std::string>();
  }
  if (vm.count("url")){
    opts.url = vm["url"].as<std::string>();
  }
  if (vm.count("facebook")) {
    opts.url = "http://graph.facebook.com/" + vm["facebook"].as<std::string>() +
      "/picture?width=1000";
  }
  if (vm.count("output")) opts.outputFilePath = vm["output"].as<std::string>();
  if (vm.count("resolution")) {
    if (vm["resolution"].as<int>() <= 0) {
      std::cout << "Resolution has to be higher than 0. Using default." << std::endl;
    } else {
      opts.resolution = vm["resolution"].as<int>();
    }
  }
  if (vm.count("palette")) {
    if (vm["palette"].as<std::string>().length() == 0) {
      std::cout << "Palette cannot be empty string. Using default." << std::endl;
    } else {
      opts.asciiPalette = vm["palette"].as<std::string>();
    }
  }
  return opts;
}

/*
 * This function makes a network call to the url and stores the response in a tmp file.
 * That file is then loaded in the destination CImg
 */
void download_image(const std::string& url, cimg_library::CImg<unsigned char>& dst) {
  std::cout << "Making a network call.." << std::endl;

  // Open filestream on new tmp file
  char tmpl[L_tmpnam] = "/var/tmp/img2ascii.XXXXXX";
  char* tmpf = mktemp(tmpl);
  std::ofstream ofs(tmpf, std::ios::binary);

  // Make the network call and write the results to a tmp file.
  try {
    curlpp::Cleanup myCleanup;
    curlpp::Easy myRequest;
    myRequest.setOpt<curlpp::options::Url>(url);
    myRequest.setOpt<curlpp::options::WriteStream>(&ofs);
    myRequest.setOpt(curlpp::options::FollowLocation(true));
    myRequest.perform();
    ofs.close();
    std::cout << "Written network response to " << tmpf << std::endl;
  } catch (curlpp::RuntimeError &e) {
    std::cerr << "Could not connect to " << url << std::endl;
    std::cout << e.what() << std::endl;
    std::remove(tmpf);
    exit(1);
  } catch (curlpp::LogicError &e) {
    std::cout << e.what() << std::endl;
    std::remove(tmpf);
    exit(1);
  }

  // Read the tmpfile into the destination cimg object
  cimg_library::cimg::exception_mode(0); // Throw exceptions quietly.
  try {
    dst.load(tmpf);
  } catch(cimg_library::CImgException &e) {
    std::cerr << "Not a valid image." << std::endl;
    std::remove(tmpf);
    exit(1);
  }
  std::remove(tmpf);
}



/*
 * =============
 * MAIN FUNCTION
 * =============
 */
int main(int argc, char *argv[])
{
  // Parsing command line arguments
  opt::variables_map vm = parse_arguments(argc, argv);
  Options opts = parse_variable_map(vm);

  // Check if input given
  if (opts.inputFilePath.empty() && opts.url.empty()) {
    std::cout << "Usage: img2ascii -f {filename} OR img2ascii -u {url}" << std::endl;
    return 1;
  }

  // Read in the file (local or network)
  cimg_library::CImg<unsigned char> src;
  if (opts.url.empty()) {
    std::cout << "Reading in " << opts.inputFilePath << std::endl;
    cimg_library::cimg::exception_mode(0); // Throw exceptions quietly.
    try {
      src.load(opts.inputFilePath.c_str());
    } catch(cimg_library::CImgException &e) {
      std::cerr << opts.inputFilePath << " is not a valid image." << std::endl;
      return 1;
    }
  } else {
    download_image(opts.url, src);
  }

  // Optimise resolution if none explicitly specified
  if (!vm.count("resolution")) {
    const float TARGET_WIDTH = 1200.0f;
    opts.resolution = ceil(src.width()/TARGET_WIDTH);
  }

  // Populate the greyscale map
  std::vector<unsigned char> greysMap = generate_greyscale_map(src, opts.resolution);

  // Stream valid ascii chars to file
  int lineWidth = src.width()/opts.resolution;
  write_ascii_art(greysMap, lineWidth, opts.outputFilePath, opts.asciiPalette);
  std::cout << "Done! You can view the art at:" << std::endl;
  std::cout << opts.outputFilePath << std::endl;
  return 0;
}
