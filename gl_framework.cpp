#include "gl_framework.hpp"
#include "arm.hpp"

extern GLfloat c_xrot,c_yrot,c_zrot;
extern bool enable_perspective,render_wireframe,show_stresses;
extern Joint* joint1;

//! Initialize GL State
void initGL(void)
{
  //Set framebuffer clear color
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //Set depth buffer furthest depth
  glClearDepth(1.0);
  //Set depth test to less-than
  glDepthFunc(GL_LESS);
  //Enable depth testing
  glEnable(GL_DEPTH_TEST);
}

//!GLFW Error Callback
void error_callback(int error, const char* description)
{
  std::cerr<<description<<std::endl;
}

//!GLFW framebuffer resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  //!Resize the viewport to fit the window size - draw to entire window
  glViewport(0, 0, width, height);
}

//!GLFW keyboard callback
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  //!Close the window if the ESC key was pressed
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  else if (key == GLFW_KEY_P && action == GLFW_PRESS)
    show_stresses = !show_stresses;   
  else if (key == GLFW_KEY_A )
    c_yrot -= 1.0;
  else if (key == GLFW_KEY_D )
    c_yrot += 1.0;
  else if (key == GLFW_KEY_W )
    c_xrot -= 1.0;
  else if (key == GLFW_KEY_S )
    c_xrot += 1.0;        
  else if (key == GLFW_KEY_Q )
    c_zrot -= 1.0;
  else if (key == GLFW_KEY_E )
    c_zrot += 1.0;   
  else if (key == GLFW_KEY_9 && action == GLFW_PRESS)
    joint1->IncWeight(-5); 
  else if (key == GLFW_KEY_0 && action == GLFW_PRESS)
    joint1->IncWeight(5);
  else if (key == GLFW_KEY_7 )
    joint1->IncAngleDegrees(-1);
  else if (key == GLFW_KEY_8 )
    joint1->IncAngleDegrees(1); 
  else if (key == GLFW_KEY_O && action == GLFW_PRESS)
    render_wireframe = !render_wireframe; 
}

  


