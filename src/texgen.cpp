//standard libary includes
#include <cmath>
#include <cstdio>
//opengl includes
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//freeimage includes
#include <FreeImage.h>
//local includes
#include "perlin.h"

const int tex_width = 512;
const int tex_height = 512;
unsigned char img[ tex_height][ tex_width][3];
char img_filename[] = "gen/img.png";
const double tau = 2 * M_PI;

void marble_gen();
void img_save( char* filename);
void img_load( char* filename);

int main( int argc, char** argv){
	marble_gen();
	img_save( img_filename);
}

void marble_gen(){
	float scale = 0.02;
	float* args = new float[ 3];
	float dx = scale / tex_width;
	float dy = scale / tex_height;
	float noise;
	for( int i = 0; i < tex_height; i++){
		args[0] = (float) ( i * dx);
		for( int j = 0; j < tex_width; j++){
			args[1] = (float) ( j * dy);
			args[2] = args[0] * args[1];
			noise = noise1( cos( 0.1 * i * j + tau / noise3( args)));
			img[i][j][0] =
			img[i][j][1] =
			img[i][j][2] =
				round( 255.0 * noise);}}
	delete[] args;}

void img_save( char* filename){
	FIBITMAP* bitmap;
	BYTE* bits;
	bitmap = FreeImage_Allocate( tex_width, tex_height, 24);
	for( int i=0; i< tex_height; i++) {
		bits = FreeImage_GetScanLine( bitmap, i);
		for( int j = 0; j < tex_width; j++) {
			bits[ FI_RGBA_RED] = img[i][j][0];
			bits[ FI_RGBA_GREEN] = img[i][j][1];
			bits[ FI_RGBA_BLUE] = img[i][j][2];
			bits += 3;}}
	FreeImage_Save( FIF_PNG, bitmap, filename, PNG_DEFAULT);}

void img_load( char* filename){
	FIBITMAP *bitmap;
	BYTE *bits;
	bitmap = FreeImage_Load( FIF_PNG, filename, PNG_DEFAULT);
	for( int i = 0; i < tex_height; i++) {
		bits = FreeImage_GetScanLine( bitmap, i);
		for( int j = 0; j < tex_width; j++){
			img[i][j][0] = bits[ FI_RGBA_RED];
			img[i][j][1] = bits[ FI_RGBA_GREEN];
			img[i][j][2] = bits[ FI_RGBA_BLUE];
			bits += 3;}}
	FreeImage_Unload( bitmap);}
