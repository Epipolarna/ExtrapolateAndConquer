#include "ModelLoader.hpp"

void ModelLoader::parseVertex(const char* line, const size_t len){
	if(line[1] == ' '){
		addVertex(line,len);
	}else if(line[1] == 'n'){
		addNormal(line,len);
	}else if(line[1] == 't'){
		addTexture(line,len);
	}else{
		printf("ERROR, unknown vertex data!\n");
		exit(0);
	}
}

void ModelLoader::addVertex(const char* line, const size_t len){
	float v1,v2,v3;
	//remove first bits of string
	char space[128];

	sscanf(line,"%c %f %f %f",space,&v1,&v2,&v3);

	vertices.push_back(QVector3D(v1,v2,v3));
}

void ModelLoader::addNormal(const char* line, const size_t len){
	float n1,n2,n3;
	//remove first bits of string
	char space[128];

	sscanf(line,"%c %f %f %f",space,&n1,&n2,&n3);

	normals.push_back(QVector3D(n1,n2,n3));
}

void ModelLoader::addTexture(const char* line, const size_t len){
	float t1,t2;
	char space[128];
	sscanf(line,"%c %f %f",space,&t1,&t2);
	textures.push_back(QVector2D(t1,t2));
}

void ModelLoader::parseFace(const char* line, const size_t len){
	char space[128];
	int f1,f2,f3,f4,f5,f6,f7,f8,f9;
	/* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */

	sscanf(line,"%c %d/%d/%d %d/%d/%d %d/%d/%d",space,&f1,&f2,&f3,&f4,&f5,&f6,&f7,&f8,&f9);

	makeVertex(f1-1,f2-1,f3-1);
	makeVertex(f4-1,f5-1,f6-1);
	makeVertex(f7-1,f8-1,f9-1);
}

bool ModelLoader::vertexExists(const int v_index, const int t_index, const int n_index, const int i){

	if(v_index < vertex.size() && t_index < texture.size() && n_index < normal.size()){
		return vertex[v_index] == vertices[i] && normal[n_index] == normals[i] && texture[t_index] == textures[i];
	}else{
		return false;
	}
}

void ModelLoader::makeVertex(const int v_index, const int t_index, const int n_index){
	//check all the current vertices if this one already exists
	for(int i=0; i < index.size(); i++){
		if(vertexExists(v_index,t_index,n_index,i)){
			index.push_back(i);
			return;
		}
	}
	vertex.push_back(vertices[v_index]);
	texture.push_back(textures[t_index]);
	normal.push_back(normals[n_index]);
	index.push_back(vertex.size() - 1);

	if(!((vertex.size() == texture.size()) && (vertex.size() == normal.size()))){
		printf("ERROR GPU LIST HAS INVALID SIZE\n");
		printf("size is: v%d, t%d, n%d", vertex.size(), texture.size(), normal.size());
		exit(0);
	}
}

void ModelLoader::parseLine(const char* line, const size_t len){
	//lowecase entire string
	char* lowerLine = new char[len];
	for(int i=0; i < len; i++){
		lowerLine[i] = tolower(line[i]);
	}

	if(lowerLine[0] == 'v'){
		parseVertex(lowerLine, len);
	}else if(lowerLine[0] == 'f'){
		parseFace(lowerLine, len);
	}else if(lowerLine[0] == '#'){

	}else{
		printf("unrecognized token, broken file? \n");
		printf("line was: %s \n", line);
	}
}

void ModelLoader::upload(void){

    VAO.create();
    VAO.bind();
    
    VBO.create();
    VBO.bind();
    VBO.allocate(vertex.constData(), vertex.size()*3*sizeof(GLfloat));
        
    NBO.create();
    NBO.bind();
    NBO.allocate(normal.constData(), normal.size()*3*sizeof(GLfloat));
    
    TBO.create();
    TBO.bind();
    TBO.allocate(texture.constData(), texture.size()*3*sizeof(GLfloat));
    

    VAO.release();
}

void ModelLoader::loadModel(const QString filename){
	FILE *modelFile;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	modelFile = fopen(filename.toStdString().c_str(),"r");
	if(modelFile == NULL){
		printf("ERROR: unable to open file: %s",filename.toStdString().c_str());
		exit(-1);
	}

	while((read = getline(&line, &len, modelFile)) != -1){
		parseLine(line,len);
	}

	//cleanup if we have to
	if(line){
		free(line);
	}

	upload();
}
