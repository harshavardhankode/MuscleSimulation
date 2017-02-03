#include "arm.hpp"


extern GLuint vPosition,vColor,uModelViewMatrix;
extern std::vector<glm::mat4> matrixStack;
extern bool render_wireframe;


Joint::Joint(float a,float a_l1, float a_l2){

	num_muscles=0;
	angle = glm::radians(a);
	l1 = a_l1;
	l2 = a_l2;
	weight = 100;

	glGenVertexArrays (1, &vao);
	glGenBuffers (1, &vbo);

}

void Joint::AddMuscle(SimMesh* muscle , float origin, float insertion, float maxForce, glm::vec3 scale){

	muscleList.push_back(muscle);
	muscleOrigins.push_back(origin);
	muscleInsertions.push_back(insertion);
	muscleMaxForces.push_back(maxForce);
	muscleActivations.push_back(0);

	float k1 = l1*(1-origin);
	float k2 = l2*(insertion);
	float n_scale = (sqrt(pow(k1,2)+pow(k2,2)-2*k1*k2*cos(angle)))/2.0;

	muscleLengths.push_back(n_scale*2.0);
	muscleScales.push_back(glm::vec3(n_scale,n_scale,n_scale));
	num_muscles++;

}


void Joint::IncAngleDegrees(float a_angle){

	double pi = 3.1415926535897;
	angle+=a_angle*(pi/180);

}
void Joint::IncAngle(float a_angle){

	angle+=a_angle;

}

void Joint::IncWeight(float wt){

	weight+=wt;

}

void Joint::CalculateActivations(){

	vector<float> moment_arms;
	float temp_moment,ms_moment_arm,net_moment_arm;
	float k1,k2;
	ms_moment_arm=0;
	for(int i=0; i<num_muscles; i++){

		k1 = l1*(1-muscleOrigins[i]);
		k2 = l2*(muscleInsertions[i]);
		temp_moment = k1 * k2 * sin(angle) / (sqrt(pow(k1,2)+pow(k2,2)-2*k1*k2*cos(angle) ) );
		//cout<<"temp_moment: "<<temp_moment<<endl;
		moment_arms.push_back(temp_moment*muscleMaxForces[i]);
		ms_moment_arm += pow(temp_moment*muscleMaxForces[i],2);

	}

	//rms_moment_arm = sqrt(rms_moment);

	net_moment_arm = weight*l2*sin(angle);

	//cout<<"nma: "<<net_moment_arm<<endl;
	//cout<<"msma: "<<ms_moment_arm<<endl;
	cout<<glm::degrees(angle)<<" "<<weight<<" ";
	for(int i=0; i<num_muscles; i++){

		muscleActivations[i] = (moment_arms[i]*net_moment_arm)/ms_moment_arm;
		//cout<<"Muscle "<<i<<" activation: "<<muscleActivations[i]<<"; ";
		if(muscleActivations[i] < 0){
			muscleActivations[i] = 0;
		}
		if(muscleActivations[i] > 1){
			muscleActivations[i] = 1;
		}

		cout<<muscleActivations[i]<<" ";
		//cout<<"Muscle "<<i<<" activation after truncating: "<<muscleActivations[i]<<"; ";

	}

	cout<<endl;

}

void Joint::Equilibriate(){
	
	float new_length,diff,k1,k2;

	CalculateActivations();



	for(int i=0; i<num_muscles; i++){
		k1 = l1*(1-muscleOrigins[i]);
		k2 = l2*(muscleInsertions[i]);
		new_length = (sqrt(pow(k1,2)+pow(k2,2)-2*k1*k2*cos(angle)));
		diff = new_length- muscleLengths[i];
		muscleLengths[i] = new_length;
		muscleList[i]->MoveInsertion(glm::vec4(diff/muscleScales[i].x,0.0,0.0,0.0));
		muscleList[i]->SetAct(muscleActivations[i]);
		muscleList[i]->Equilibriate(1,0.001);

	}

}

void Joint::Render(){

	// Add each vertex of faces in the order into the render buffer (TODR)
	render_buffer.clear();

	render_buffer.push_back(glm::vec4(0.0,0.0,0.0,1.0));
	render_buffer.push_back(glm::vec4(0.0,l1,0.0,1.0));
	render_buffer.push_back(glm::vec4(0.0,0.0,0.0,1.0));
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f,0.0f,-1.0f));
	render_buffer.push_back(rotation*glm::vec4(0.0,l2,0.0,1.0));
	/*for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(curr_vertices[faces[i].x]);
		render_buffer.push_back(curr_vertices[faces[i].y]);
		render_buffer.push_back(curr_vertices[faces[i].z]);

		//cout<<faces[i].x<<","<<faces[i].y<<","<<faces[i].z<<endl;
	}*/

	//setting up colour

	render_buffer.push_back(glm::vec4(1.0,1.0,1.0,1.0));
	render_buffer.push_back(glm::vec4(1.0,1.0,1.0,1.0));
	render_buffer.push_back(glm::vec4(1.0,1.0,1.0,1.0));
	render_buffer.push_back(glm::vec4(1.0,1.0,1.0,1.0));
	/*for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(red_blue(forces[faces[i].x].norm(),500,2000));
		render_buffer.push_back(red_blue(forces[faces[i].y].norm(),500,2000));
		render_buffer.push_back(red_blue(forces[faces[i].z].norm(),500,2000));

	}*/

	// setup the vbo and vao;
	//cout<<render_buffer.size()<<endl;
	//bind them
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	glBufferData (GL_ARRAY_BUFFER, render_buffer.size() * sizeof(glm::vec4), &render_buffer[0], GL_STATIC_DRAW);
	//cout<< render_buffer.size() * sizeof(glm::vec4)<<endl;
	//setup the vertex array as per the shader
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	size_t offset = (1.0/2)*render_buffer.size()* sizeof(glm::vec4);

	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset));


	glm::mat4* ms_mult = multiply_stack(matrixStack);

	glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));




	glBindVertexArray (vao);

	/*if(render_wireframe){
		for(int i = 0; i < num_faces*3; i += 3)
	 		glDrawArrays(GL_LINE_LOOP, i, 3);
	}
	else{
		glDrawArrays(GL_TRIANGLES, 0, num_faces*3);
	}*/

	glDrawArrays(GL_LINES, 0, 4);

	delete ms_mult;

	float pi = 3.1415926535897;
	float k1,k2,d,muscle_angle;
	for(int i=0; i<num_muscles; i++){
		k1 = l1*(1-muscleOrigins[i]);
		k2 = l2*(muscleInsertions[i]);
		d = (sqrt(pow(k1,2)+pow(k2,2)-2*k1*k2*cos(angle)));

		muscle_angle = pi*0.5 - asin(  k2*sin(angle)/(d));

		if(pow(k1,2)+pow(d,2)<pow(k2,2))
			muscle_angle = -muscle_angle;
		//cout<<muscle_angle<<endl;

		matrixStack.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,l1*(1-muscleOrigins[i]),0.0f)));
			
	 	matrixStack.push_back(glm::rotate(glm::mat4(1.0f),muscle_angle, glm::vec3(0.0f,0.0f,-1.0f)) );
	 	
	 	matrixStack.push_back(glm::scale(glm::mat4(1.0f),glm::vec3(muscleScales[i])));
	 	
	 	muscleList[i]->Render();

		matrixStack.pop_back();
		matrixStack.pop_back();
		matrixStack.pop_back();
	 }


}