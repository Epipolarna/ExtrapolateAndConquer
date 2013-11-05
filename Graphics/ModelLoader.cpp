#include "ModelLoader.hpp"

ModelLoader::ModelLoader(void){
	VBO = QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
	IBO = QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);

	VBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	TBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	NBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	IBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
}

void ModelLoader::dumpData(void){
	printf("vertices\n");
	for(QVector3D v: vertex){
		printf("%f,%f,%f \n",v.x(),v.y(),v.z());
	}

	printf("normals\n");
	for(QVector3D v: normals){
		printf("%f,%f,%f \n",v.x(),v.y(),v.z());
	}

	printf("textures\n");
	for(QVector2D v: textures){
		printf("%f,%f \n",v.x(),v.y());
	}
}


bool ModelLoader::readVertex(const QStringList data){
	GLfloat f[3];
	int j = 0;
	bool ok = false;
	float ff = 0;
	for(int i=0; i < data.size(); i++){
		ff = data.at(i).toFloat(&ok);
		if(ok){
			f[j] = ff;
			j = j + 1;
		}
	}

    if(j == 3){
		vertices.push_back(QVector3D(f[0],f[1],f[2]));
		return true;
	}else{
		return false;
	}
}

bool ModelLoader::readTexture(const QStringList data){
	GLfloat f[2];
	int j = 0;
	bool ok = false;
	float ff = 0;
	for(int i=0; i < data.size(); i++){
		ff = data.at(i).toFloat(&ok);
		if(ok){
			f[j] = ff;
			j = j + 1;
		}
	}
    if(j == 2){
		textures.push_back(QVector2D(f[0],f[1]));
		return true;
	}else{
		return false;
	}
	return false;
}

bool ModelLoader::readNormal(const QStringList data){
	GLfloat f[3];
	int j = 0;
	bool ok = false;
	float ff = 0;
	for(int i=0; i < data.size(); i++){
		ff = data.at(i).toFloat(&ok);
		if(ok){
			f[j] = ff;
			j = j + 1;
		}
	}
    if(j == 3){
		normals.push_back(QVector3D(f[0],f[1],f[2]));
		return true;
	}else{
		return false;
	}
}


int ModelLoader::vertexExists(const QVector3D v, const QVector3D n, const QVector2D t){

    for(int i=0; i < index.size() && i < vertex.size(); i++){
        if(vertex[i] == v && normal[i] == n && texture[i] == t){
            return i;
        }
    }
    return 0;
}

void ModelLoader::makeVertex(const QStringList data){
    int dex[3];
    int j=0;
    for(int i=0; i < data.size(); i++){
        bool success = false;
        int number = data.at(i).toInt(&success);
        if(success){
            //start counting from 0 instead of 1
            dex[j] = number - 1;
            j++;
        }
    }

    if(j == 2){
        printf("CASE NOT IMPLEMENTED! \n");
        exit(0);
        //only normal and vertex
        /*
        QVector3D v = vertices[dex[0]];
        QVector2D n = normals[dex[1]];
        vertex.push_back(v);
        normal.push_back(n);
        */
    }else if(j == 3){
        QVector3D v = vertices[dex[0]];
        QVector2D t = textures[dex[1]];
        QVector3D n = normals[dex[2]];
        //vertex,normal and texture
        int pos = vertexExists(v,n,t);
        if(pos){
            index.push_back(pos);
        }else{
            vertex.push_back(v);
            normal.push_back(n);
            texture.push_back(t);
            index.push_back(vertex.size()-1);
        }
    }else{
        printf("Invalid index specification!\n");
        exit(0);
    }
}

bool ModelLoader::readFace(const QStringList data){

    for(int i=0; i < data.size(); i++){
        if(data.at(i).contains("/")){
            makeVertex(data.at(i).split("/"));
        }
    }
    return true;
}

void ModelLoader::parseLine(QString line){
	QStringList bits = line.split(" ");
	
	bool parseOk = false;

	if(bits.at(0).compare("v") == 0){
		parseOk = readVertex(bits);
	}else if(bits.at(0).compare("vt") == 0){
		parseOk = readTexture(bits);
	}else if(bits.at(0).compare("vn") == 0){
		parseOk = readNormal(bits);
	}else if(bits.at(0).compare("f") == 0){
		parseOk = readFace(bits);
	}else if(bits.at(0).compare("#") == 0){
		//comment int the obj
		parseOk = true;
    }else if(bits.at(0).compare("g") == 0){
        //group information
        parseOk = true;
    }else if(bits.at(0).compare("#mtllib") == 0){
        //material data?
        parseOk = true;
    }else if(bits.at(0).compare("#usemtl") == 0){
        //material information for vertices?
        parseOk = true;
	}else{
		printf("Unknown token! \n string was: %s",line.toStdString().c_str());
		parseOk = true;
	}

	if(!parseOk){
		printf("Failed to parse line:%s",line.toStdString().c_str());
		for(int i=0; i < bits.size();i++){
			printf("%s,",bits.at(i).toStdString().c_str());
		}
		exit(0);
	}
}

void ModelLoader::upload(void){
	//dumpData();

    VAO.create();
    VAO.bind();
    
    VBO.create();
    VBO.bind();
    VBO.allocate(vertex.constData(), vertex.size()*sizeof(QVector3D));
        
    NBO.create();
    NBO.bind();
    NBO.allocate(normal.constData(), normal.size()*sizeof(QVector3D));
    
    TBO.create();
    TBO.bind();
    TBO.allocate(texture.constData(), texture.size()*sizeof(QVector2D));
    
    IBO.create();
    IBO.bind();
    IBO.allocate(index.constData(),index.size()*3*sizeof(GLuint));

    VAO.release();
}

void ModelLoader::loadModel(const QString filename){
	
	QFile file(filename);

	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		printf("Failed to open file: %s \n",filename.toStdString().c_str());
	}

	while(!file.atEnd()){
		QByteArray rawLine = file.readLine();
		QString line = QString(rawLine).toLower();

		parseLine(line);
	}

	upload();
}
