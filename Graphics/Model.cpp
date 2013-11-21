#include "Model.hpp"

Model::Model(void){
	VBO = QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
	IBO = QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);

	VBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	TBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	NBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
	IBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
}


void Model::modelFromData(const QVector<QVector3D> vertices, const QVector<QVector3D> normals, const QVector<QVector2D> textures, const QVector<unsigned int> indices){
    vertex = QVector<QVector3D>(vertices);
    normal = QVector<QVector3D>(normals);
    texture = QVector<QVector2D>(textures);
    index = QVector<unsigned int>(indices);

    upload();
}

void Model::dumpData(void){
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


bool Model::readVertex(const QStringList data){
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

bool Model::readTexture(const QStringList data){
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
	//some pepole are idiots and provide 3d texture coordinates
	//so we have to handle both cases
	//but we always discard the third coordinate
    if(j == 2 || j == 3){
		textures.push_back(QVector2D(f[0],f[1]));
		return true;
	}else{
		return false;
	}
	return false;
}

bool Model::readNormal(const QStringList data){
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


int Model::vertexExists(const QVector3D v, const QVector3D n, const QVector2D t){

    for(int i=0; i < index.size() && i < vertex.size(); i++){
        if(vertex[i] == v && normal[i] == n && texture[i] == t){
            return i;
        }
    }
    return 0;
}

void Model::makeVertex(const QStringList data){
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

bool Model::readFace(const QStringList data){

    for(int i=0; i < data.size(); i++){
        if(data.at(i).contains("/")){
            makeVertex(data.at(i).split("/"));
        }
    }
    return true;
}

void Model::parseLine(QString line){

	QStringList bits = line.split(" ",QString::SkipEmptyParts);
	
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
    }else if(bits.at(0).size() == 1){
    	parseOk = true;
	}else{
		parseOk = false;
	}

	lineCounter = lineCounter + 1;
	if(!parseOk){
		printf("Failed to parse line:%d \n line was\n",lineCounter);
		printf("%s",line.toStdString().c_str());
		printf("size of chunk was: %d \n",bits.size());
		for(int i=0; i < bits.size();i++){
			printf("%s,",bits.at(i).toStdString().c_str());
		}
		exit(0);
	}
}

void Model::upload(void){
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
    IBO.allocate(index.constData(),index.size()*sizeof(GLuint));

    VAO.release();
}


void Model::unitizeModel(){
	//compute max/min on each dimension
	float maxX,maxY,maxZ;
	float minX,minY,minZ;

	//sane initials..
	maxX = minX = vertex[0].x();
	maxY = minY = vertex[0].y();
	maxZ = minZ = vertex[0].z();

	for(QVector3D &v : vertex){
		if(maxX < v.x()){
			maxX = v.x();
		}
		if(maxY < v.y()){
			maxY = v.y();
		}
		if(maxZ < v.z()){
			maxZ = v.z();
		}

		if(minX > v.x()){
			minX = v.x();
		}
		if(minY > v.y()){
			minY = v.y();
		}
		if(minZ > v.z()){
			minZ = v.z();
		}
	}

	//dimensions
	float w = std::abs(maxX) - std::abs(minX);
	float h = std::abs(maxY) - std::abs(minY);
	float d = std::abs(maxZ) - std::abs(minZ);

	//get model center
	float cx = w / 2.0;
	float cy = h / 2.0;
	float cz = d / 2.0;

	//compute scaling factor..
	float max = std::max(w, std::max(h,d));
	float scale = 2.0 / max;

	printf("untizing scale was %f, max was: %f \n",scale,max);
	//translate and scale vertices
	for(QVector3D &v : vertex){
		v = QVector3D((v.x()-cx)*scale,
					  (v.y()-cy)*scale,
					  (v.z()-cz)*scale);

	}
}

void Model::loadModel(const QString filename, const bool unitize){
	
	QFile file(filename);
	lineCounter = 0;
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		printf("Failed to open file: %s \n",filename.toStdString().c_str());
	}

	while(!file.atEnd()){
		QByteArray rawLine = file.readLine();
		QString line = QString(rawLine).toLower();

		parseLine(line);
	}

	if(unitize){
		printf("unitizing model \n");
		unitizeModel();
	}

	upload();
	vertices.resize(0);
	normals.resize(0);
	textures.resize(0);
}
