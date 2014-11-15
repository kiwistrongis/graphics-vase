//standard libary includes
#include <cstdlib>
#include <cstring>
//freeimage includes
#include <FreeImage.h>
//local includes
#include "texture.h"

char posx_file[] = "posx.jpg";
char negx_file[] = "negx.jpg";
char posz_file[] = "posz.jpg";
char negz_file[] = "negz.jpg";
char posy_file[] = "posy.jpg";
char negy_file[] = "negy.jpg";


struct Texture* loadJpeg( char* filename){
	FIBITMAP* bitmap = FreeImage_Load( FIF_JPEG, filename, JPEG_DEFAULT);
	struct Texture* result = new Texture;

	(*result).width = FreeImage_GetWidth( bitmap);
	(*result).height = FreeImage_GetHeight( bitmap);
	(*result).depth = FreeImage_GetBPP( bitmap)/8;
	int size = (*result).width * (*result).height * (*result).depth;
	(*result).size = size;
	unsigned char* data = (unsigned char*) malloc( size);
	(*result).data = data;

	int k = 0;
	BYTE* bits;
	for( int j = 0; j < (*result).height; j++){
		bits = FreeImage_GetScanLine( bitmap, j);
			for( int i = 0; i < (*result).width; i++){
				data[ k++] = bits[ FI_RGBA_RED];
				data[ k++] = bits[ FI_RGBA_GREEN];
				data[ k++] = bits[ FI_RGBA_BLUE];
				bits += 3;}}

	return result;}

struct Texture* loadPng( char* filename){
	FIBITMAP* bitmap = FreeImage_Load( FIF_PNG, filename, PNG_DEFAULT);
	struct Texture* result = new Texture;

	(*result).width = FreeImage_GetWidth( bitmap);
	(*result).height = FreeImage_GetHeight( bitmap);
	(*result).depth = FreeImage_GetBPP( bitmap)/8;
	int size = (*result).width * (*result).height * (*result).depth;
	(*result).size = size;
	unsigned char* data = (unsigned char*) malloc( size);
	(*result).data = data;

	int k = 0;
	BYTE* bits;
	for( int j = 0; j < (*result).height; j++){
		bits = FreeImage_GetScanLine( bitmap, j);
			for( int i = 0; i < (*result).width; i++){
				data[ k++] = bits[ FI_RGBA_RED];
				data[ k++] = bits[ FI_RGBA_GREEN];
				data[ k++] = bits[ FI_RGBA_BLUE];
				bits += 3;}}

	FreeImage_Unload( bitmap);
	return result;}
