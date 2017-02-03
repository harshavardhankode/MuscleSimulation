#ifndef _MESH_HPP_
#define _MESH_HPP_

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
#include "glm/gtx/string_cast.hpp"
#include "Eigen/Dense"
#include "Eigen/SVD"
#include <math.h>
#include <algorithm> 

using namespace std;


struct tet_t{

	glm::vec4 vertices;	
	
	Eigen::Matrix3f F; // Deformation gradient

	Eigen::Matrix3f P; //Piola kirchoff stress

	Eigen::Vector3f g[4]; // Stress -> Force coefficients for each node

};

/*struct node_t{

	glm::vec3 force;

};*/



class SimMesh{


	GLuint vao,vbo;

	int num_vertices, num_faces, num_tets;

	std::vector< glm::vec4 > vertices;
	std::vector< glm::vec4 > curr_vertices;
	std::vector< glm::vec4 > final_vertices;
	std::vector< float > vertex_stress;

	std::vector< glm::vec3 > faces;
	std::vector< glm::vec4 > tets;

	std::vector< Eigen::Vector3f > forces;

	std::vector< tet_t* > sim_tets;

	std::vector< glm::vec4 > render_buffer;

	int origin_vertex;
	int insertion_vertex;

	float act,l,Tmax,l_init; //activation,rest length, max active stress;

	float avg_move,max_move;

	int iteration;
	public:
		SimMesh();

		void ReadTetgenMesh(string mesh_name);
	
		void Render();

		void RenderWireframe();

		void RenderSlice();

		void CalculateDeformationGradients();

		void CalculateStresses();

		void ComputeForces();		

		void TimeStep(float dT);

		void MoveInsertion(glm::vec4 t);

		void Equilibriate(float threshold, float dT);

		void SetAct(float a);
		void SetLen(float a_l);
		void SetTmax(float a_Tmax);

};

glm::mat4* multiply_stack(std::vector<glm::mat4> matStack);

#endif