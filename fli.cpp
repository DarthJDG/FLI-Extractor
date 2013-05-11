#include "fli.h"

FLI_Player::FLI_Player()
{
}

FLI_Player::FLI_Player(char *filename)
{
    Open(filename);
}

FLI_Player::~FLI_Player()
{
	if(IsOpen) Close();
}

bool FLI_Player::Open(char *filename)
{
	if(IsOpen) Close();

	FLIFile = fopen(filename,"rb");
	if(FLIFile == NULL) return false;

    IsOpen = true;

	// Initialize Palette
	for(int i = 0; i < 256; i++)
	{
		Pal.Colors[i].R = i;
		Pal.Colors[i].G = i;
		Pal.Colors[i].B = i;
	}

	// Read Header
	fread(&Header,128,1,FLIFile);

	if(Header.magic != 0xAF11)
	{
		Close();
		return false;
	}

	CurrentFrame = new unsigned char[Header.width * Header.height];

	FLI_Chunk dummy;
	Process_FLI_BLACK(dummy);
	AnimFrame = 0;
}

void FLI_Player::Process_FLI_COLOR(FLI_Chunk chunk)
{
	uint16_t cnt;
	uint8_t skip;
	uint8_t chg;
	uint16_t change;
	FLI_Color col;

	int palpos = 0;
	if(fread(&cnt,2,1,FLIFile))
	{
		for(int packet = 0; packet < cnt; packet++)
		{
			fread(&skip,1,1,FLIFile);
			fread(&chg,1,1,FLIFile);
			palpos += skip;
			change = chg;
			if(!change) change = 256;
			for(int j = 0; j < change; j++)
			{
				fread(&col,3,1,FLIFile);
				col.R *= 4;
				col.G *= 4;
				col.B *= 4;
				Pal.Colors[palpos] = col;
				palpos++;
			}
		}
	}
}

void FLI_Player::Process_FLI_LC(FLI_Chunk chunk)
{
	uint16_t y;
	uint16_t lineoffset;
	uint16_t offset;
	uint16_t change;
	uint8_t packets;
	uint8_t skip;
	int8_t size;
	uint8_t pixel;
	uint8_t buf[256];

	fread(&y,2,1,FLIFile);
	lineoffset = y * Header.width;

	fread(&change,2,1,FLIFile);

	for(int line = 0; line < change; line++)
	{
		offset = lineoffset;
		fread(&packets,1,1,FLIFile);
		for(int p = 0; p < packets; p++)
		{
			fread(&skip,1,1,FLIFile);
			fread(&size,1,1,FLIFile);
			offset += skip;
			if(size != 0)
			{
				if(size > 0)
				{
					fread(buf,size,1,FLIFile);
					for(int n = 0; n < size; n++)
						CurrentFrame[offset++] = buf[n];
				}
				else
				{
					fread(&pixel,1,1,FLIFile);
					for(int n = 0; n < -size; n++)
						CurrentFrame[offset++] = pixel;
				}
			}
		}
		lineoffset += Header.width;
	}
}

void FLI_Player::Process_FLI_BLACK(FLI_Chunk chunk)
{
	int size = Header.width * Header.height;
	for(int i = 0; i < size; i++)
		CurrentFrame[i] = 0;
}

void FLI_Player::Process_FLI_BRUN(FLI_Chunk chunk)
{
	uint16_t lineoffset = 0;
	uint16_t offset;
	uint16_t change;
	uint8_t packets;
	uint8_t skip;
	int8_t size;
	uint8_t pixel;
	uint8_t buf[256];

	change = Header.height;

	for(int line = 0; line < change; line++)
	{
		offset = lineoffset;
		fread(&packets,1,1,FLIFile);
		for(int p = 0; p < packets; p++)
		{
			fread(&size,1,1,FLIFile);
			if(size != 0)
			{
				if(size > 0)
				{
					fread(&pixel,1,1,FLIFile);
					for(int n = 0; n < size; n++)
						CurrentFrame[offset++] = pixel;
				}
				else
				{
					fread(buf,-size,1,FLIFile);
					for(int n = 0; n < -size; n++)
						CurrentFrame[offset++] = buf[n];
				}
			}
		}
		lineoffset += Header.width;
	}
}

void FLI_Player::Process_FLI_COPY(FLI_Chunk chunk)
{
	int size = Header.width * Header.height;
	fread(CurrentFrame,size,1,FLIFile);
}

bool FLI_Player::NextFrame()
{
	if(AnimFrame == Header.frames) return false;

	char dummy;
	int i;
	FLI_Frame frame;
	FLI_Chunk chunk;
	uint16_t cursize;
	if(!fread(&frame,16,1,FLIFile)) return false;

	if(frame.size == 0) return true;

	for(int n = 0; n < frame.chunks; n++)
	{
		if(!fread(&chunk,6,1,FLIFile)) return false;

		switch(chunk.type)
		{
			case FLI_COLOR:
				Process_FLI_COLOR(chunk);
				break;

			case FLI_LC:
				Process_FLI_LC(chunk);
				break;

			case FLI_BLACK:
				Process_FLI_BLACK(chunk);
				break;

			case FLI_BRUN:
				Process_FLI_BRUN(chunk);
				break;

			case FLI_COPY:
				Process_FLI_COPY(chunk);
				break;
		}
		i = ftell(FLIFile);
		if(i%2) fread(&dummy,1,1,FLIFile);
	}

	AnimFrame++;
	return true;
}

void FLI_Player::RenderFrame(uint8_t *buffer) {
	int idx;

	for(int n = 0; n < 64000; n++)
	{
		idx = CurrentFrame[n];
		buffer[n * 3] = Pal.Colors[idx].R;
		buffer[n * 3 + 1] = Pal.Colors[idx].G;
		buffer[n * 3 + 2] = Pal.Colors[idx].B;
	}
}

int FLI_Player::GetPos()
{
    return AnimFrame;
}

void FLI_Player::Close()
{
	fclose(FLIFile);
	delete CurrentFrame;
	IsOpen = false;
}

