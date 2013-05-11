#ifndef FLI_H
#define FLI_H

#include <stdio.h>
#include <stdint.h>

#define FLI_COLOR 11
#define FLI_LC    12
#define FLI_BLACK 13
#define FLI_BRUN  15
#define FLI_COPY  16

struct FLI_Header
{
	uint32_t size;
	uint16_t magic;
	uint16_t frames;
	uint16_t width;
	uint16_t height;
	uint16_t depth;
	uint16_t flags;
	uint16_t speed;
	uint32_t next;
	uint32_t frit;
	uint8_t expand[102];
};

struct FLI_Frame
{
	uint32_t size;
	uint16_t magic;
	uint16_t chunks;
	uint8_t expand[8];
};

struct FLI_Chunk
{
	uint32_t size;
	uint16_t type;
};

struct FLI_Color
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

struct FLI_Palette
{
	FLI_Color Colors[256];
};

class FLI_Player
{
	protected:

	FILE *FLIFile;
	bool IsOpen = false;
	int AnimFrame;

	unsigned char *CurrentFrame;
	FLI_Palette Pal;
	FLI_Header Header;

	void Process_FLI_COLOR(FLI_Chunk chunk);
	void Process_FLI_LC(FLI_Chunk chunk);
	void Process_FLI_BLACK(FLI_Chunk chunk);
	void Process_FLI_BRUN(FLI_Chunk chunk);
	void Process_FLI_COPY(FLI_Chunk chunk);

	public:

	FLI_Player();
	FLI_Player(char *filename);
	~FLI_Player();

	bool Open(char *filename);
	bool NextFrame();
	void RenderFrame(uint8_t *buffer);
	int GetPos();
	void Close();
};

#endif
