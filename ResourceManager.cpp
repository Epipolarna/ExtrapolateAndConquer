#include "ResourceManager.hpp"


const QString ResourceManager::resourcePath = "./resources/";
const QString ResourceManager::modelPath = resourcePath + "models/";
const QString ResourceManager::shaderPath = resourcePath + "shaders/";
const QString ResourceManager::texturePath = resourcePath + "textures/";

ResourceManager::ResourceManager(){
    initializeOpenGLFunctions();
}

ResourceManager::~ResourceManager(){
    //destroy all resources etc
}

Model* ResourceManager::getModel(QString name){
	return models[name];
}

GLuint ResourceManager::getTexture(QString name){
	return textures[name];
}

QOpenGLShaderProgram* ResourceManager::getShader(QString name){
    if(shaders.find(name) != shaders.end()){
        return shaders[name];
    }else{
        printf("shader %s not found!",name.toStdString().c_str());
        exit(0);
    }
}

bool ResourceManager::loadModel(QString modelName){
	Model *ml = new Model();
	ml->loadModel(modelPath+modelName+".obj");

    models[modelName] = ml;
	//TODO unhappy case
	return true;
}

bool ResourceManager::loadShader(QString shaderName){
	QOpenGLShaderProgram* shader = new QOpenGLShaderProgram;
	QString vertexPath = shaderPath + shaderName + ".vert";
	QString fragmentPath = shaderPath + shaderName + ".frag";

    shader->addShaderFromSourceFile(QOpenGLShader::Vertex,  vertexPath);
    shader->addShaderFromSourceFile(QOpenGLShader::Fragment,fragmentPath);
    shader->link();

    shaders[shaderName] = shader;
    
    int glError = glGetError();
    if(glError == 0 && shader->isLinked() == true){
    	return true;
    }else{
        printf("error loading shader %s ", shaderName.toStdString().c_str());
        printf("error code was: %x",glError);
        exit(0);
    	return false;
    }
}

bool ResourceManager::loadTexture(QString textureName, bool mipmap){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    QImage image(texturePath+textureName+".png");
    QImage imageGL = QGLWidget::convertToGLFormat(image);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageGL.width(), imageGL.height(),
                 0, GL_RGBA, GL_UNSIGNED_BYTE, imageGL.bits());

    if(mipmap){
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int errorCode = glGetError();
    if(errorCode != 0){
    	qDebug() << "error loading texture";
        return false;
    }

    textures[textureName] = textureID;

    return true;
}
