#ifndef texture_h
#define texture_h

struct Texture {
	int width;
	int height;
	int depth;
	int size;
	unsigned char* data;
};

struct Texture* loadPng( char* filename);
struct Texture* loadJpeg( char* filename);

#endif
