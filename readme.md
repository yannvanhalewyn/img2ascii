IMG2ASCII
=========

This is a little fun project I made in an afternoon. My goal was to try to make a simple program that would take an image as input, and spit out a rendering in ASCII art of that image. I wanted to do this so I could learn more about image processing (I used a simple greyscaling algorithm). Later on I decided it would be a great networking exercise if I could get the program to take in a url of an image instead of a file, and do the same thing. I have no idea if I took the right approach, but it does work and I learned a lot.

Installing
----------

There's a simple make file in the repo. Mac user's can just run 'make' to build the project. Dependencies are:
  * boost       - for boost::program_options
  * curlpp      - a library for c++ to execute HTTP requests
  * cimg        - A c++ library for parsing images

Usage
-----

NOTE: Best results are attained when letting the software optimize the resolution by itself. This aims for a textfile with a width of about 1000 chars. Opening the textfile in text-edit or notepad, and reducing the font-size to a minimum is required to enjoy this beautiful art.

Here's a simple usage example:

`img2ascii -f ~/path/to/img.jpg` => this will create a output.txt file in the working directory
`img2ascii -u http://url.to.img.jpg` => this will download the image to a temp file, read from it, delete that temp file and save the ASCII art to output.txt

The options:
* `-f [ --file ] arg`
The input file path in jpeg format. If a file is given, any passed in url's will be ignored.
* `-u [ --url ] arg`
A url hosting the image.
* `-o [ --output ] arg`
The output file path.
* `-r [ --resolution ] arg`
The chunk resolution. How many pixels will go in one output character. Cannot be a negative number.
* `--palette arg`
A custom string representing all ASCII characters to be used in the output. Order the characters from 'dark' to 'light', e.g.: '@O=-*,. '. Cannot be empty.
* `--help ]`
Produce help message`



