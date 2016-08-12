#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "mesh.hpp"

//Globals in this file

#ifndef _GLOBALS_
#define _GLOBALS_


GLuint vPosition,vColor,uModelViewMatrix;

GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 2.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;

bool enable_perspective=false;
bool render_wireframe = false;
std::vector<glm::mat4> matrixStack;

glm::mat4* multiply_stack(std::vector<glm::mat4> matStack){
	glm::mat4* mult;
	mult = new glm::mat4(1.0f);

	for(int i=0;i<matStack.size();i++){
		*mult = (*mult) * matStack[i];
	}	

	return mult;
}



SimMesh* mesh1;

#endif