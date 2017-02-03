#include "geometry.hpp"

GLuint shaderProgram;

//glm::mat4 view_matrix;

glm::mat4 rotation_matrix;
glm::mat4 projection_matrix;
glm::mat4 c_rotation_matrix;
glm::mat4 lookat_matrix;

glm::mat4 model_matrix;
glm::mat4 view_matrix;


glm::mat4 modelview_matrix;

void initBuffersGL(void)
{

  // Load shaders and use the resulting shader program
  std::string vertex_shader_file("vshader.glsl");
  std::string fragment_shader_file("fshader.glsl");

  std::vector<GLuint> shaderList;
  shaderList.push_back(csX75::LoadShaderGL(GL_VERTEX_SHADER, vertex_shader_file));
  shaderList.push_back(csX75::LoadShaderGL(GL_FRAGMENT_SHADER, fragment_shader_file));

  shaderProgram = csX75::CreateProgramGL(shaderList);
  glUseProgram( shaderProgram );

  // getting the attributes from the shader program
  vPosition = glGetAttribLocation( shaderProgram, "vPosition" );
  vColor = glGetAttribLocation( shaderProgram, "vColor" ); 
  uModelViewMatrix = glGetUniformLocation( shaderProgram, "uModelViewMatrix");

  //Initializing buffers of the mesh also here

  mesh1 = new SimMesh();
  mesh2 = new SimMesh();
  mesh1->ReadTetgenMesh("./test/muscle1.2");
  mesh2->ReadTetgenMesh("./test/muscle1.2");

  joint1 = new Joint(90,1,1);
  joint1->AddMuscle(mesh1,0.1,0.2,1000,glm::vec3(0.4,0.4,0.4));
  joint1->AddMuscle(mesh2,0.8,0.8,500,glm::vec3(0.4,0.4,0.4));
  //joint1


}

void renderGL(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //view_matrix = glm::ortho(-1.0, 3.0, -2.0, 2.0, -2.0, 2.0);

  //glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(view_matrix));


  
  matrixStack.clear();

  //Creating the lookat and the up vectors for the camera
  c_rotation_matrix = glm::rotate(glm::mat4(1.0f), glm::radians(c_xrot), glm::vec3(1.0f,0.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_yrot), glm::vec3(0.0f,1.0f,0.0f));
  c_rotation_matrix = glm::rotate(c_rotation_matrix, glm::radians(c_zrot), glm::vec3(0.0f,0.0f,1.0f));

  glm::vec4 c_pos = glm::vec4(c_xpos,c_ypos,c_zpos, 1.0)*c_rotation_matrix;
  glm::vec4 c_up = glm::vec4(c_up_x,c_up_y,c_up_z, 1.0)*c_rotation_matrix;
  //Creating the lookat matrix
  lookat_matrix = glm::lookAt(glm::vec3(c_pos),glm::vec3(0.0),glm::vec3(c_up));

  //creating the projection matrix
  if(enable_perspective)
    projection_matrix = glm::frustum(-7.0, 7.0, -7.0, 7.0, 1.0, 7.0);
    //projection_matrix = glm::perspective(glm::radians(90.0),1.0,0.1,5.0);
  else
    projection_matrix = glm::ortho(-1.0, 3.0, -2.0, 2.0, 0.1, 10.0);

  view_matrix = projection_matrix*lookat_matrix;

  matrixStack.push_back(view_matrix);

  //glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(view_matrix));

  //matrixStack.push_back(glm::rotate(glm::mat4(1.0f),(float)glm::radians(90.0), glm::vec3(0.0f,0.0f,-1.0f)) );
  //matrixStack.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,2.0f,0.0f)));

  //mesh1->Render();
  joint1->Render();

}


int main(int argc, char** argv)
{
  //! The pointer to the GLFW window
  GLFWwindow* window;

  //! Setting up the GLFW Error callback
  glfwSetErrorCallback(error_callback);

  //! Initialize GLFW
  if (!glfwInit())
    return -1;

  //We want OpenGL 4.0
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //This is for MacOSX - can be omitted otherwise
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
  //We don't want the old OpenGL 
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 

  //! Create a windowed mode window and its OpenGL context
  window = glfwCreateWindow(512, 512, "geometry viewer", NULL, NULL);
  if (!window)
    {
      glfwTerminate();
      return -1;
    }
  
  //! Make the window's context current 
  glfwMakeContextCurrent(window);

  //Initialize GLEW
  //Turn this on to get Shader based OpenGL
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err)
    {
      //Problem: glewInit failed, something is seriously wrong.
      std::cerr<<"GLEW Init Failed : %s"<<std::endl;
    }

  //Print and see what context got enabled
  std::cout<<"Vendor: "<<glGetString (GL_VENDOR)<<std::endl;
  std::cout<<"Renderer: "<<glGetString (GL_RENDERER)<<std::endl;
  std::cout<<"Version: "<<glGetString (GL_VERSION)<<std::endl;
  std::cout<<"GLSL Version: "<<glGetString (GL_SHADING_LANGUAGE_VERSION)<<std::endl;

  //Keyboard Callback
  glfwSetKeyCallback(window, key_callback);
  //Framebuffer resize callback
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  //Initialize GL state
  initGL();
  initBuffersGL();


  int temp;
  bool incflag=true;
  temp = 0;
  int act = 0;
  /*for(int wt=1;wt<178;wt++){

    joint1->IncAngleDegrees(1);
    joint1->CalculateActivations();
  }*/
  
  //mesh1->SetAct(0.5);

  // Loop until the user closes the window
  while (glfwWindowShouldClose(window) == 0)
    {

      //mesh1->Equilibriate(0.0005,0.001);
      //mesh1->Equilibriate(1,0.001);
      joint1->Equilibriate();

      // if(incflag){
      //   mesh1->MoveInsertion(glm::vec4(0.05,0,0,0));
      //   temp++;
      //   if(temp>10){
      //     incflag=false;
      //   }
      // } else {
      //   mesh1->MoveInsertion(glm::vec4(-0.05,0,0,0));
      //   temp--;
      //   if(temp<0){
      //     incflag=true;
      //   }
      // }

      
      // if(incflag){
      //   mesh1->SetAct(act/10.0);
      //   act++;
      //   if(act>=10){
      //     incflag=false;
      //   }
      // } else {
      //   mesh1->SetAct(act/10.0);
      //   act--;
      //   if(act<=0){
      //     incflag=true;
      //   }
      // }

      // mesh1->CalculateDeformationGradients();
      // mesh1->CalculateStresses();
      // mesh1->ComputeForces();
      // // Render here
      // mesh1->TimeStep(0.001);
      // iteration++;
      renderGL();
      //sleep(1);
      
      // Swap front and back buffers
      glfwSwapBuffers(window);
      // Poll for and process events
      glfwPollEvents();

    }
  

  glfwTerminate();
  return 0;
}