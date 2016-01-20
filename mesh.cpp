
#include "mesh.hpp"


extern GLuint vPosition,vColor,uModelViewMatrix;
//extern std::vector<glm::mat4> matrixStack;

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
				cout<<num_faces<<endl;
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

	//Create and Load buffers

	//cout<<vertices.size()<<endl;
	// Add each vertex of faces in the order into the render buffer

	for( int i=0; i< faces.size() ; i++){
		render_buffer.push_back(vertices[faces[i].x]);
		render_buffer.push_back(vertices[faces[i].y]);
		render_buffer.push_back(vertices[faces[i].z]);

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

	//glEnableVertexAttribArray( vColor );
	//glVertexAttribPointer( vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertex_buffer_size));
}

void SimMesh::Render(){

	//glm::mat4* ms_mult = multiply_stack(matrixStack);

	//glUniformMatrix4fv(uModelViewMatrix, 1, GL_FALSE, glm::value_ptr(*ms_mult));
	glBindVertexArray (vao);
	glDrawArrays(GL_TRIANGLES, 0, num_faces*3);

	// for memory 
	//delete ms_mult;

}