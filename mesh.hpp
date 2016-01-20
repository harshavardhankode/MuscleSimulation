#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gl_framework.hpp"
#include "shader_util.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

class SimMesh{


	GLuint vao,vbo;

	int num_vertices, num_faces;

	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec3 > faces;

	std::vector< glm::vec4 > render_buffer;


	public:
		SimMesh();

		void ReadTetgenMesh(string mesh_name);



		void Render();



};