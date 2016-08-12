
#include "mesh.hpp"


extern GLuint vPosition,vColor,uModelViewMatrix;
//extern std::vector<glm::mat4> matrixStack;
extern bool render_wireframe;

glm::vec4 red_blue(float value, float lo, float hi){
	float f;
	//cout<<f<<endl;
	if(value<lo){
		f = 0.0;
	}
	else if (value>hi){
		f = 1.0;
	}
	else{
		f = (value-lo)/(hi - lo);
	}
	//cout<<f<<endl;
	return glm::vec4(f,1.0-f,0.0,1.0);
	//return glm::vec4(1.0,1.0,0.0,1.0);
}

SimMesh::SimMesh(){

	// initialize the vaos and vbos here

	//Ask GL for a Vertex Attribute Objects (vao)
	glGenVertexArrays (1, &vao);
	//Ask GL for aVertex Buffer Object (vbo)
	glGenBuffers (1, &vbo);

}

void SimMesh::ReadTetgenMesh(string mesh_name){

	//Read vertices
	string line;
	stringstream temp_ss;

	ifstream node_file((mesh_name+".node").c_str());

	if (node_file.is_open())
	{
		while ( getline (node_file,line) )
		{	
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				temp_ss.str(line);
				temp_ss >> num_vertices;
				break;
			}
		}
		while ( getline (node_file,line) ){
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				temp_ss.str(line);
				int counter;				
				glm::vec4 vertex;
				temp_ss >> counter >> vertex.x >> vertex.y >> vertex.z;
				vertex.w = 1;
				vertices.push_back(vertex);
			}

		}

	}
	else{
		cout<<"could not open mesh node file\n";
	}

	//Read faces
	ifstream face_file((mesh_name + ".face").c_str());

	if (face_file.is_open())
	{
		while ( getline (face_file,line) )
		{	
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				temp_ss.str(line);
				temp_ss >> num_faces;
				//cout<<num_faces<<endl;
				break;
			}
		}
		while ( getline (face_file,line) ){
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				line +=" ";
				temp_ss.str(line);
				int counter;				
				glm::vec3 face;
				temp_ss >> counter >> face.x >> face.y >> face.z;
				// indices in vertices array start from 0
				face.x--;face.y--;face.z--;
				faces.push_back(face);
			}

		}	
	}


	//Read Tetrahedra
	ifstream tets_file((mesh_name + ".ele").c_str());

	if (tets_file.is_open())
	{
		while ( getline (tets_file,line) )
		{	
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				temp_ss.str(line);
				temp_ss >> num_tets;
				//cout<<num_tets<<endl;
				break;
			}
		}
		while ( getline (tets_file,line) ){
			if(line[line.find_first_not_of(" ")] == '#' )
				continue;
			else{
				line +=" ";
				temp_ss.str(line);
				int counter;				
				glm::vec4 tet;
				temp_ss >> counter >> tet.x >> tet.y >> tet.z >> tet.w;
				// indices in vertices array start from 0
				tet.x--;tet.y--;tet.z--;tet.w--;
				tets.push_back(tet);
			}

		}	
	}

	// initialize sim tets
	for(int i=0;i<num_tets;i++){
		tet_t* temp = new tet_t;
		temp->vertices = tets[i];
		sim_tets.push_back(temp);

		// initialize the stress-force coefficients
		Eigen::Vector3f v0;
		v0 << vertices[temp->vertices[0]].x ,vertices[temp->vertices[0]].y ,vertices[temp->vertices[0]].z;
		Eigen::Vector3f v1;
		v1 << vertices[temp->vertices[1]].x ,vertices[temp->vertices[1]].y ,vertices[temp->vertices[1]].z;
		Eigen::Vector3f v2;
		v2 << vertices[temp->vertices[2]].x ,vertices[temp->vertices[2]].y ,vertices[temp->vertices[2]].z;
		Eigen::Vector3f v3;
		v3 << vertices[temp->vertices[3]].x ,vertices[temp->vertices[3]].y ,vertices[temp->vertices[3]].z;

		temp->g[0] = -(1.0/6.0)*((v2-v0).cross(v1-v0) + (v1-v0).cross(v3-v0) + (v3-v0).cross(v2-v0));
		temp->g[1] = -(1.0/6.0)*((v3-v1).cross(v0-v1) + (v0-v1).cross(v2-v1) + (v2-v1).cross(v3-v1));
		temp->g[2] = -(1.0/6.0)*((v3-v2).cross(v1-v2) + (v1-v2).cross(v0-v2) + (v0-v2).cross(v3-v2));
		temp->g[3] = -(1.0/6.0)*((v1-v3).cross(v2-v3) + (v2-v3).cross(v0-v3) + (v0-v3).cross(v1-v3));


	}

	curr_vertices = final_vertices = vertices; // No deformation for now

}

void SimMesh::Render(){


	//cout<<vertices.size()<<endl;
	// Add each vertex of faces in the order into the render buffer
	render_buffer.clear();

	for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(curr_vertices[faces[i].x]);
		render_buffer.push_back(curr_vertices[faces[i].y]);
		render_buffer.push_back(curr_vertices[faces[i].z]);

		//cout<<faces[i].x<<","<<faces[i].y<<","<<faces[i].z<<endl;
	}

	//setting up colour

	for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(red_blue(forces[faces[i].x].norm(),500,2000));
		render_buffer.push_back(red_blue(forces[faces[i].y].norm(),500,2000));
		render_buffer.push_back(red_blue(forces[faces[i].z].norm(),500,2000));

	}

	// setup the vbo and vao;
	//cout<<render_buffer.size()<<endl;
	//bind them
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	glBufferData (GL_ARRAY_BUFFER, render_buffer.size() * sizeof(glm::vec4), &render_buffer[0], GL_STATIC_DRAW);
	//cout<< render_buffer.size() * sizeof(glm::vec4)<<endl;
	//setup the vertex array as per the shader
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	size_t offset = (1.0/2)*render_buffer.size()* sizeof(glm::vec4);

	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(offset) );


	//glm::mat4* ms_mult = multiply_stack(matrixStack);

	//glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));

	glBindVertexArray (vao);

	if(render_wireframe){
		for(int i = 0; i < num_faces*3; i += 3)
	 		glDrawArrays(GL_LINE_LOOP, i, 3);
	}
	else{
		glDrawArrays(GL_TRIANGLES, 0, num_faces*3);
	}

	// for memory 
	//delete ms_mult;

}

void SimMesh::RenderWireframe(){


	//cout<<vertices.size()<<endl;
	// Add each vertex of faces in the order into the render buffer
	render_buffer.clear();

	for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(vertices[faces[i].x]);
		render_buffer.push_back(vertices[faces[i].y]);
		render_buffer.push_back(vertices[faces[i].z]);

		//cout<<faces[i].x<<","<<faces[i].y<<","<<faces[i].z<<endl;
	}

	//setting up colour
	for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(red_blue(forces[faces[i].x].norm(),3000,3500));
		render_buffer.push_back(red_blue(forces[faces[i].y].norm(),3000,3500));
		render_buffer.push_back(red_blue(forces[faces[i].z].norm(),3000,3500));

		//cout<<faces[i].x<<","<<faces[i].y<<","<<faces[i].z<<endl;
	}
	// setup the vbo and vao;
	//cout<<render_buffer.size()<<endl;
	//bind them
	glBindVertexArray (vao);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);

	glBufferData (GL_ARRAY_BUFFER, render_buffer.size() * sizeof(glm::vec4), &render_buffer[0], GL_STATIC_DRAW);

	//setup the vertex array as per the shader
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	glEnableVertexAttribArray( vColor );
	glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET((1/2)*render_buffer.size()* sizeof(glm::vec4)));


	//glm::mat4* ms_mult = multiply_stack(matrixStack);

	//glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));
	glBindVertexArray (vao);
	
	for(int i = 0; i < num_faces*3; i += 3)
 		glDrawArrays(GL_LINE_LOOP, i, 3);

	// for memory; 
	// delete ms_mult;
}

void SimMesh::CalculateDeformationGradients(){

	for(int i=0;i<num_tets;i++){

		Eigen::Matrix3f Dm;
		Eigen::Matrix3f Ds;
		Eigen::Matrix3f TempF;

		glm::vec4 edge1 = curr_vertices[sim_tets[i]->vertices[1]] -curr_vertices[sim_tets[i]->vertices[0]];
		glm::vec4 edge2 = curr_vertices[sim_tets[i]->vertices[2]] -curr_vertices[sim_tets[i]->vertices[0]];
		glm::vec4 edge3 = curr_vertices[sim_tets[i]->vertices[3]] -curr_vertices[sim_tets[i]->vertices[0]];
		
		Dm <<edge1.x,edge2.x,edge3.x,edge1.y,edge2.y,edge3.y,edge1.z,edge2.z,edge3.z;

		edge1 = final_vertices[sim_tets[i]->vertices[1]] -final_vertices[sim_tets[i]->vertices[0]];
		edge2 =	final_vertices[sim_tets[i]->vertices[2]] -final_vertices[sim_tets[i]->vertices[0]];
		edge3 = final_vertices[sim_tets[i]->vertices[3]] -final_vertices[sim_tets[i]->vertices[0]];

		Ds <<edge1.x,edge2.x,edge3.x,edge1.y,edge2.y,edge3.y,edge1.z,edge2.z,edge3.z;


		//Checking for inversion
		//sim_tets[i]->F = Ds*Dm.inverse();
		TempF = Ds*Dm.inverse();
		// if(TempF.determinant() < 0){ //= 0 case?
		// 	Eigen::JacobiSVD<Eigen::Matrix3f> svd(TempF, Eigen::ComputeFullU | Eigen::ComputeFullV);
			
		// 	Eigen::Vector3f Sv = svd.singularValues();
		// 	Eigen::Matrix3f U = svd.matrixU();
		// 	Eigen::Matrix3f V = svd.matrixV();
		// 	cout << "Its singular values are:" << endl << Sv << endl;
		// 	cout << "U matrix:" << endl << U << endl << U.determinant() << endl;
		// 	cout << "V matrix:" << endl << V << endl << V.determinant() << endl;
		
		// 	int minC;
		// 	Sv.minCoeff(&minC);
		// 	Sv[minC]= - Sv[minC];
		// 	cout << "Its new values are:" << endl << Sv << endl;
		// 	Eigen::Matrix3f S = Sv.asDiagonal();
		// 	cout << "S matrix after invert:" << endl << S << endl;

		// 	TempF = U*S*V.transpose();
		// }
		sim_tets[i]->F = TempF;
		//cout<<sim_tets[i]->F<<endl;

	}

}

void SimMesh::CalculateStresses(){

	Eigen::Vector3f fm ; // fiber direction 
	fm<<1,0,0; // initialised to x direction, teporary
	// TODO later add fiber direction in each tet

	Eigen::Matrix3f F,F_orig,P,P_r;
	Eigen::Matrix3f C; // Cauchy stress
	Eigen::Matrix3f I1;
	float Jc,Jcc,w1,w2,w12,i1,lambda,p,pf;
	float mat_c1 = 30000; // (muscle)
	//float mat_c1 = 60000 // (tendon),
	float mat_c2 = 10000; // (muscle and tendon),
	float K = 60000; // (muscle)
	//float K = 80000;// (tendon)
	float T = 80000;

	for(int i=0;i<num_tets;i++){

		F_orig = sim_tets[i]->F;

		if( true /*F_orig.determinant() < 0*/){ 
			Eigen::JacobiSVD<Eigen::Matrix3f> svd(F_orig, Eigen::ComputeFullU | Eigen::ComputeFullV);
			
			Eigen::Vector3f Sv = svd.singularValues();
			Eigen::Matrix3f U = svd.matrixU();
			Eigen::Matrix3f V = svd.matrixV();
			// cout << "Its singular values are:" << endl << Sv << endl;
			// cout << "U matrix:" << endl << U << endl << U.determinant() << endl;
			// cout << "V matrix:" << endl << V << endl << V.determinant() << endl;
			F = Sv.asDiagonal();

			bool Vflag = false; // Represents if V has been negated i.e one column inverted
			// Making V a rotation
			if(V.determinant() < 0){
				V.col(0) = -V.col(0);
				Vflag = true;
				cout << "V matrix inverted to make it rotation:" << endl << V << endl << V.determinant() << endl;
				cout<<"Dets of F and U:" << F.determinant() << " and " <<U.determinant()<<endl;
			}

			// Using the method given in the paper to find U
			//U = F_orig* V * F.inverse();

			// Inverting corresponding column in U and corresponding column in F, but maintaining sign
			if(U.determinant() < 0 & !Vflag){

				int minC;
				Sv.minCoeff(&minC);
				Sv[minC]= - Sv[minC];
				F = Sv.asDiagonal();
				cout << "F matrix in the rotated space, after invering the least element:" << endl << F << endl;

				U.col(minC) = -U.col(minC);
				cout << "U matrix inverted to make it rotation:" << endl << U << endl << U.determinant() << endl;

			}

			if(U.determinant() < 0 & Vflag){
				U.col(0) = -U.col(0);
			}
			//}


			Jc = F.determinant();

			if(Jc < 0){
				cout<<"Invert"<<endl;
			}

			Jc = 1.0/pow(Jc,(1.0/3));
			Jcc = pow(Jc,2);
			C = F.transpose() * F;
			I1 = Jcc*C;
			i1 = I1.trace();
			lambda = sqrt(fm.transpose() * C * fm);

			w1 = 4*Jcc*mat_c1;
			w2 = 4*pow(Jcc,2)*mat_c2;
			w12 = w1 + i1*w2;

			p = K * log(Jc); // Jc / Jcc
			pf = (1.0/3.0)*(w12*C.trace() - w2*((C*C).trace()) + T*pow(lambda,2));

			P_r = w12*F - w2*(F*F*F) + (p - pf)*(F.inverse()) + 4*Jcc*T*(F*fm)*fm.transpose();

			P = U * P_r * V.transpose();

			sim_tets[i]->P = P;
			//std::cout<<P<<endl;
		}
		else{

			F = F_orig;
			Jc = F.determinant();

			if(Jc < 0){
				cout<<"Invert"<<endl;
			}

			Jc = 1.0/pow(Jc,(1.0/3));
			Jcc = pow(Jc,2);
			C = F.transpose() * F;
			I1 = Jcc*C;
			i1 = I1.trace();
			lambda = sqrt(fm.transpose() * C * fm);

			w1 = 4*Jcc*mat_c1;
			w2 = 4*pow(Jcc,2)*mat_c2;
			w12 = w1 + i1*w2;

			p = K * log(Jc); // Jc / Jcc
			pf = (1.0/3.0)*(w12*C.trace() - w2*((C*C).trace()) + T*pow(lambda,2));

			P = w12*F - w2*(F*F*F) + (p - pf)*(F.inverse()) + 4*Jcc*T*(F*fm)*fm.transpose();
			sim_tets[i]->P = P;

		}
	}
}

void SimMesh::ComputeForces(){

	forces.clear();
	for(int i=0;i<num_vertices;i++){
		Eigen::Vector3f a;
		a<<0,0,0;
		forces.push_back(a);
	}
	tet_t* curr_tet;
	
	for(int i=0;i<num_tets;i++){

		curr_tet = sim_tets[i];
		int curr_vertex;
		for(int j=0;j<4;j++){

			curr_vertex = curr_tet->vertices[j];
			forces[curr_vertex]-=(curr_tet->P)*(curr_tet->g[j]);

		}

	}

	// for(int i=0;i<num_vertices;i++){
	// 	std::cout<<forces[i].norm()<<endl;
	// }

}




void SimMesh::TimeStep(float dT){

	// 1 and 18 are clamped = 0 & 17 in the array
	glm::vec4 curr_force;
	for(int i=0;i<num_vertices;i++){

		if(i==0 || i==17){
			continue;
		}
		curr_force = glm::vec4(forces[i][0],forces[i][1],forces[i][2],1.0);
		curr_vertices[i] = curr_vertices[i] + curr_force*dT*dT*1.0f; //Later implement the mass here
		// if(i==0){

		// 	std::cout<<glm::to_string(curr_force*dT*dT*1.0f)<<endl;
		// }

	}
}