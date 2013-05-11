#include <iostream>
#include <windows.h>
#include <png/png.hpp>

#include "fli.h"

using namespace std;

int main(int argc, char* argv[])
{
	cout << "FLI Extractor v0.1 - Saves each animation frame into a PNG image." << endl << endl;

	if(argc < 2) {
		cout << "Usage:" << endl << endl << "fli-extractor [options] [filename]" << endl << endl;
		cout << "It will create a folder matching the filename of the animation and saves all" << endl << "images in there." << endl << endl;
		cout << "Options:" << endl << endl << "--delta      PNG images will contain only the pixels that have changed since" << endl;
		cout << "             last frame, the rest is alpha.";
		return 1;
	}

	bool option_delta = false;
	for(int i = 1; i < argc - 1; i++) {
		if(strcmp(argv[i], "--delta") == 0) option_delta = true;
	}

	cout << "Creating folder... ";
	char *filename = argv[argc - 1];
	char foldername[255];
	strcpy(foldername, filename);
	for(int i = 0; i < 255; i++) {
		if(foldername[i] == '.') {
			foldername[i] = '\0';
			break;
		}
	}
	CreateDirectory(foldername, NULL);
	cout << "done." << endl;

	cout << "Extracting animation frames... ";

	// Create buffers for RGB frames
	uint8_t *frontbuffer = new uint8_t[320 * 200 * 3];
	uint8_t *backbuffer = new uint8_t[320 * 200 * 3];

	// Pointers to swap around
	uint8_t *current = frontbuffer;
	uint8_t *prev = backbuffer;
	uint8_t *temp;

	// Clear backbuffer
	for(int i = 0; i < 320 * 200 * 3; i++) {
		backbuffer[i] = 0;
	}

	FLI_Player *player = new FLI_Player(filename);

	int frame = 0;
	while(player->NextFrame()) {
		frame++;
		player->RenderFrame(current);

		int offset = 0;
		png::image< png::rgba_pixel > image(320, 200);
		for (size_t y = 0; y < image.get_height(); ++y)
		{
			for (size_t x = 0; x < image.get_width(); ++x)
			{
				if(!option_delta) {
					image[y][x] = png::rgba_pixel(current[offset], current[offset + 1], current[offset + 2], 255);
				} else if((frame == 1) || (current[offset] != prev[offset]) || (current[offset + 1] != prev[offset + 1]) || (current[offset + 2] != prev[offset + 2])) {
					image[y][x] = png::rgba_pixel(current[offset], current[offset + 1], current[offset + 2], 255);
				} else {
					image[y][x] = png::rgba_pixel(0, 0, 0, 0);
				}
				offset += 3;
			}
		}

		char imagefile[255];
		sprintf(imagefile, "%s/%04d.png", foldername, frame);
		image.write(imagefile);

		// Swap buffers
		temp = current;
		current = prev;
		prev = temp;
	}

	// Free memory
	delete frontbuffer;
	delete backbuffer;

	cout << "done." << endl;

    return 0;
}
