#ifndef _JOINT_HPP_
#define _JOINT_HPP_


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
#include "mesh.hpp"


using namespace std;

class Joint{

	GLuint vao,vbo;
	int num_muscles;
	float angle,l1,l2; // angle in radians, length in metres
	float weight;
	vector<SimMesh*> muscleList;
	vector<float> muscleOrigins;	//0-1
	vector<float> muscleInsertions;
	vector<float> muscleActivations; //0-1
	vector<float> muscleMaxForces;
	vector<float> muscleLengths;

	//If dummies are to be added
	vector<float> dummy_muscle_moments;
	vector<float> dummy_muscle_max_force;

	vector<glm::vec3> muscleScales;
	std::vector< glm::vec4 > render_buffer;
public:

	Joint(float,float,float);
	void AddMuscle(SimMesh* , float, float, float,glm::vec3);

	void IncAngleDegrees(float angle);
	void IncAngle(float angle);
	void IncWeight (float wt);

	void Equilibriate();
	void Render();

	void CalculateActivations();
	
	//void Step();

};

#endif