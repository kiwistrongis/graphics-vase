//standard libary includes
#include <algorithm>
#include <cmath>
#include <cstdio>
//core opengl includes
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
//glm includes
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
//freeimage includes
#include <FreeImage.h>
//local includes
#include "shaders.h"
#include "texture.h"
#include "../lib/tiny_obj_loader.h"

//window vars
int window;
char window_name[] = "model_test";
int window_width = 800;
int window_height = 600;
int display0_width = 1600;
int display0_height = 900;
int display1_width = 1920;
int display1_height = 1200;
bool display1_connected = false;
int window_x = display1_connected ?
	display0_width + ( display1_width - window_width) / 2 :
	( display0_width - window_width) / 2;
int window_y = display1_connected ?
	( display1_height - window_height) / 2 :
	( display0_height - window_height) / 2;
//camera stuff
const double pi = M_PI;
const double half_pi = M_PI / 2;
const double tau = 2 * M_PI;
double camera_r =
	//8.0;
	250.0;
double theta = 0.0;
double phi = 0.0;
float near_plane = 1.0f;
float far_plane =
	//100.0f;
	400.0f;
//shaders
int shader_program = 0;
char vertshdr_file[] = "shdr/model_test.vert.glsl";
char fragshdr_file[] = "shdr/model_test.frag.glsl";
char texture_file[] =
	//"gen/marble.png";
	"gen/wood.png";
	//"data/checkerboard.png";
char object_file[] =
	//"data/cube.obj";
	"data/vase.obj";
//matices and stuff
glm::vec3 camera_eye;
glm::vec3 camera_center;
glm::vec3 camera_up;
glm::mat4 projection;
//consts
const bool shader_debug = false;
const bool model_debug = false;
//other
GLuint model_vao;
struct Texture* texture;
tinyobj::mesh_t model_mesh;
size_t model_vcount;

//init stuff
void model_load();
void vars_init();
void window_init( int argc, char** argv);
void shaders_init();
void vao_init();
//destructors
void vars_destroy();
//event functions
void display();
void resize( int width_new, int height_new);
void keydown( unsigned char key, int x, int y);
//other
void camera_recalc();

int main( int argc, char** argv){
	//init
	model_load();
	vars_init();
	window_init( argc, argv);
	shaders_init();
	vao_init();

	//start main loop
	glutMainLoop();}

void model_load(){
	printf("loading model\n");
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string err = tinyobj::LoadObj(
		shapes, materials, object_file);

	if( ! err.empty()){
		printf("error loading model:\n%s\n", err.c_str());
		exit( 1);}

	//copy relevant data
	int shape_count = shapes.size();
	if( shape_count != 1){
		printf("error loading model: more than one shape\n");
		exit( 1);}
	model_mesh = shapes.at( 0).mesh;
	model_vcount = model_mesh.indices.size();
	printf("vert count: %d\n", (uint) model_mesh.positions.size() / 3);
	printf("norm count: %d\n", (uint) model_mesh.normals.size() / 3);
	printf("index count: %d\n", (uint) model_mesh.indices.size() / 3);

	//debug model info
	if( model_debug){
		printf("dumping vertices:\n");
		for( uint i = 0; i < model_mesh.positions.size(); i+= 3)
			printf("  vert %03d: %+09.4f %+09.4f %+09.4f\n", i/3,
				model_mesh.positions.at( i + 0),
				model_mesh.positions.at( i + 1),
				model_mesh.positions.at( i + 2));
		printf("\n");

		printf("dumping normals:\n");
		for( uint i = 0; i < model_mesh.normals.size(); i+= 3)
			printf("  norm %03d: %+07.4f %+07.4f %+07.4f\n", i/3,
				model_mesh.normals.at( i + 0),
				model_mesh.normals.at( i + 1),
				model_mesh.normals.at( i + 2));
		printf("\n");

		printf("dumping indices\n");
		for( uint i = 0; i < model_mesh.indices.size(); i+= 3)
			printf("  index %03d: %03d %03d %03d\n", i/3,
				model_mesh.indices.at( i + 0),
				model_mesh.indices.at( i + 1),
				model_mesh.indices.at( i + 2));
		printf("\n");}

	printf("model loaded\n\n");}

void vars_init(){
	camera_eye = glm::vec3( 0.0f, 0.0f, 0.0f);
	camera_center = glm::vec3( 0.0f, 0.0f, 0.0f);
	camera_up = glm::vec3( 0.0f, 0.0f, 1.0f);
	camera_recalc();}

void window_init( int argc, char** argv){
	printf("initializing window\n");
	//create window
	glutInit( &argc, argv);
	glutInitWindowSize( (GLsizei) window_width, (GLsizei) window_height);
	glutInitDisplayMode( GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition( window_x, window_y);
	window = glutCreateWindow( window_name);
	GLenum error = glewInit();
	if( error != GLEW_OK){
		printf("Error starting GLEW: %s\n",
			glewGetErrorString( error));
		exit( 1);}

	//connect event functions
	glutDisplayFunc( display);
	glutReshapeFunc( resize);
	glutKeyboardFunc( keydown);
	glutIgnoreKeyRepeat( true);
	atexit( vars_destroy);

	//set other flags
	glEnable( GL_DEPTH_TEST);
	glClearColor( 0.08, 0.08, 0.08, 1.0);}

void shaders_init(){
	printf("building vertex shader\n");
	int vertshdr = build_shader( GL_VERTEX_SHADER, vertshdr_file);
	printf("building fragment shader\n");
	int fragshdr = build_shader( GL_FRAGMENT_SHADER, fragshdr_file);
	printf("building shader program\n");
	shader_program = build_program( vertshdr, fragshdr);
	if( shader_debug){
		printf("dumping shader program debug info\n");
		dump_program( shader_program);}}

void vao_init(){
	printf("loading vao\n");
	//vars
	GLuint vertex_buffer;
	GLuint index_buffer;
	GLuint texture_buffer;
	GLint vertex_in;
	GLint normal_in;
	GLint tex_in;
	GLint texbind_in;
	//gen vao
	glGenVertexArrays( 1, &model_vao);
	glBindVertexArray( model_vao);

	//get data counts
	size_t vert_count = model_mesh.positions.size() / 3;
	size_t norm_count = model_mesh.normals.size() / 3;
	size_t face_count = model_mesh.indices.size() / 3;
	size_t texbind_count = model_mesh.texcoords.size() / 2;
	printf("vert_count: %lu\n", vert_count);
	printf("norm_count: %lu\n", norm_count);
	printf("face_count: %lu\n", face_count);
	printf("texbind_count: %lu\n", texbind_count);

	//geometry
	GLfloat* verts = new GLfloat[ 4 * vert_count];
	GLfloat* norms = new GLfloat[ 3 * norm_count];
	GLfloat* texbinds = new GLfloat[ 2 * texbind_count];
	GLushort* faces = new GLushort[ 3 * face_count];
	size_t verts_size = 4 * sizeof( GLfloat) * vert_count;
	size_t norms_size = 3 * sizeof( GLfloat) * norm_count;
	size_t texbinds_size = 2 * sizeof( GLfloat) * texbind_count;
	size_t faces_size = 3 * sizeof( GLushort) * face_count;

	//load geometry from mesh
	// load verts
	for( size_t i = 0; i < vert_count; i++){
		verts[ 4 * i + 0] = model_mesh.positions.at( 3 * i + 0);
		verts[ 4 * i + 1] = model_mesh.positions.at( 3 * i + 1);
		verts[ 4 * i + 2] = model_mesh.positions.at( 3 * i + 2);
		verts[ 4 * i + 3] = 1.0;}
	// load norms
	for( size_t i = 0; i < norm_count; i++){
		norms[ 3 * i + 0] = model_mesh.normals.at( 3 * i + 0);
		norms[ 3 * i + 1] = model_mesh.normals.at( 3 * i + 1);
		norms[ 3 * i + 2] = model_mesh.normals.at( 3 * i + 2);}
	// load texbinds
	for( size_t i = 0; i < texbind_count; i++){
		texbinds[ 2 * i + 0] = model_mesh.texcoords.at( 2 * i + 0);
		texbinds[ 2 * i + 1] = model_mesh.texcoords.at( 2 * i + 1);}
	// load indices
	for( size_t i = 0; i < face_count; i++){
		faces[ 3 * i + 0] = (GLushort) model_mesh.indices.at( 3 * i + 0);
		faces[ 3 * i + 1] = (GLushort) model_mesh.indices.at( 3 * i + 1);
		faces[ 3 * i + 2] = (GLushort) model_mesh.indices.at( 3 * i + 2);}

	//bind vertex buffer
	glGenBuffers( 1, &vertex_buffer);
	glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData( GL_ARRAY_BUFFER,
		verts_size + norms_size + texbinds_size,
		NULL, GL_STATIC_DRAW);
	//buffer verts
	glBufferSubData( GL_ARRAY_BUFFER,
		0, verts_size, verts);
	//buffer norms
	glBufferSubData( GL_ARRAY_BUFFER,
		verts_size, norms_size, norms);
	//buffer texbinds
	glBufferSubData( GL_ARRAY_BUFFER,
		verts_size + norms_size, texbinds_size, texbinds);

	//buffer index buffer
	glGenBuffers( 1, &index_buffer);
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData( GL_ELEMENT_ARRAY_BUFFER,
		faces_size, faces, GL_STATIC_DRAW);

	//load and buffer texture
	texture = loadPng( texture_file);
	glGenTextures( 1, &texture_buffer);
	glActiveTexture( GL_TEXTURE0);
	glBindTexture( GL_TEXTURE_2D, texture_buffer);
	glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA8,
		texture->width, texture->height);
	glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0,
		texture->width, texture->height, GL_RGB,
		GL_UNSIGNED_BYTE, texture->data);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//use shader program
	glUseProgram( shader_program);

	//bind vertex
	vertex_in = glGetAttribLocation( shader_program,"vertex_in");
	glVertexAttribPointer( vertex_in, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray( vertex_in);

	//bind normals
	normal_in = glGetAttribLocation( shader_program, "normal_in");
	glVertexAttribPointer( normal_in, 3, GL_FLOAT, GL_FALSE,
		0, (void*) verts_size);
	glEnableVertexAttribArray( normal_in);

	//bind texbinds
	texbind_in = glGetAttribLocation( shader_program, "texbind_in");
	glVertexAttribPointer( texbind_in, 2, GL_FLOAT, GL_FALSE,
		0, (void*) ( verts_size + norms_size));
	glEnableVertexAttribArray( texbind_in);

	//bind texture
	tex_in = glGetUniformLocation( shader_program, "tex_in");
	glUniform1i( tex_in, 0);

	//destroy temp arrays
	/*delete[] verts;
	delete[] norms;
	delete[] texbinds;
	delete[] faces;*/
	
	printf("done loading vao\n");}

void vars_destroy(){}

void resize( int width_new, int height_new){
	if( height_new == 0)
		height_new = 1;
	window_width = width_new;
	window_height = height_new;
	float ratio = 1.0 * window_width / window_height;
	glViewport( 0, 0,
		(GLsizei) window_width, (GLsizei) window_height);
	projection = glm::perspective( 45.0f, ratio, near_plane, far_plane);}

void display(){
	//clear buffer
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram( shader_program);

	//set up view
	//glLoadIdentity();
	camera_recalc();
	glm::mat4 view = glm::lookAt(
		camera_eye, camera_center, camera_up);

	//bind vars
	int view_loc = glGetUniformLocation( shader_program, "model_view");
	glUniformMatrix4fv( view_loc, 1, 0, glm::value_ptr( view));
	int proj_loc = glGetUniformLocation( shader_program, "projection");
	glUniformMatrix4fv( proj_loc, 1, 0, glm::value_ptr( projection));

	//draw cube
	glBindVertexArray( model_vao);
	glDrawElements(
		GL_TRIANGLES, model_vcount, GL_UNSIGNED_SHORT, NULL);

	//swap buffers
	glutSwapBuffers();}

void keydown( unsigned char key, int x, int y){
	//printf( "key pressed: %c %d\n", key, key);
	switch( key){
		case 'a':
			theta -= tau / 16;
			camera_recalc();
			glutPostRedisplay();
			break;
		case 'd':
			theta += tau / 16;
			camera_recalc();
			glutPostRedisplay();
			break;
		case 'w':
			phi += tau / 16;
			camera_recalc();
			glutPostRedisplay();
			break;
		case 's':
			phi -= tau / 16;
			camera_recalc();
			glutPostRedisplay();
			break;
		case 'r':
			theta = 0.0;
			phi = 0.0;
			camera_recalc();
			glutPostRedisplay();
			break;
		case 'q':
		case 27:
			printf( "exiting\n");
			exit( 0);}}

void camera_recalc(){
	if( phi > half_pi) phi = half_pi;
	if( phi < - half_pi) phi = - half_pi;
	camera_eye[0] = camera_r * cos( phi) * cos( theta);
	camera_eye[1] = camera_r * cos( phi) * sin( theta);
	camera_eye[2] = camera_r * sin( phi);}
