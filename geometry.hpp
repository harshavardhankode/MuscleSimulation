#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef _GLOBALS_
#define _GLOBALS_

#include "glm/gtc/type_ptr.hpp"
#include "mesh.hpp"
#include "arm.hpp"
//Globals in this file




GLuint vPosition,vColor,uModelViewMatrix;

GLfloat c_xpos = 0.0, c_ypos = 0.0, c_zpos = 2.0;
GLfloat c_up_x = 0.0, c_up_y = 1.0, c_up_z = 0.0;
GLfloat c_xrot=0.0,c_yrot=0.0,c_zrot=0.0;

bool enable_perspective=false;
bool render_wireframe = false;
bool show_stresses=false;
std::vector<glm::mat4> matrixStack;
int iteration=0;





SimMesh* mesh1,*mesh2;
Joint* joint1;

#endif