# FLI Extractor
*Extract FLI animation frames to PNG images*

Contents:

1. What is FLI Extractor?
2. Usage
3. License and copyright

## 1. What is FLI Extractor?

The sole purpose of FLI extractor is to extract animation frames from legacy FLI animation files into PNG images. It is a Windows only application written in C++, with no effort to make it cross-platform. Can be compiled with Code Blocks and MinGW, provided that you install the required libraries: zlib, libpng and png++.

Releases can be found in the release directory.

To learn why this application was created, read the post at [CodeBin](http://codebin.co.uk/blog/extract-fli-animation-frames-to-png/).

## 2. Usage

```fli-extractor [options] [filename]```

The only option at the moment is ```--delta``` which will make each pixel of the PNG image transparent where nothing was changed from the previous frame. The program will create a folder matching the name of the FLI input file and will extract all frames there with zero padded filenames, starting with ```0001.png```.

## 3. License and copyright

This program is available under GPL3, please see the LICENSE and COPYRIGHT files for details.
