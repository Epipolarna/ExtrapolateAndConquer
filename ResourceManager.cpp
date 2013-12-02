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

bool ResourceManager::loadModel(QString modelName, bool unitize){
	Model *ml = new Model();
	ml->loadModel(modelPath+modelName+".obj",unitize);

    models[modelName] = ml;
    //TODO unhappy cases
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

#include <string>
cv::Mat cvmat_from_qimage(const QImage& qimage)
{
    cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4, (uchar*)qimage.bits(), qimage.bytesPerLine());
    cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
    int from_to[] = { 0,0,  1,1,  2,2 };
    cv::mixChannels( &mat, 1, &mat2, 1, from_to, 3 );
    return mat2;
}

QImage qimage_from_cvmat(const cv::Mat& mat)
{
    cv::Mat rgb;
    cvtColor(mat, rgb, CV_BGR2RGBA);
    return QImage((const unsigned char*)(rgb.data), rgb.cols, rgb.rows, QImage::Format_RGB888);
}

bool ResourceManager::loadTextureAtlas(QString textureAtlasName, int atlasSize, bool mipmap){

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    QImage image(texturePath+textureAtlasName+".png");
    QImage imageGL = QGLWidget::convertToGLFormat(image);

    if(mipmap){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // Generate mip-map for each sub-image
        int width = imageGL.width()/2;
        int height = imageGL.height()/2;
        int level = 1;

        cv::Mat nextImage = cvmat_from_qimage(imageGL);
        QString str = texturePath+textureAtlasName+".png";
        std::string path = std::string((char*)str.data());
        //cv::Mat nextImage = cv::imread(path, CV_LOAD_IMAGE_COLOR);
        //cvtColor(nextImage.clone(), nextImage, CV_BGR2RGB);
        cv::imshow("original", nextImage);
        while(std::min(width,height) > 0) {/*
            // Downsample the texture
            std::cerr << std::min(width,height);
            cv::resize(nextImage, nextImage, cv::Size(width, height));
            cv::imshow("level"+std::to_string(level), nextImage);
            cv::waitKey(1);

            // Filter each texture in the atlas separately
            if(std::min(width,height) > 0)
            {
                int scaleFactorW = width/atlasSize;
                int scaleFactorH = height/atlasSize;
                for(int x = 0; x < atlasSize; x++)
                    for(int y = 0; y < atlasSize; y++)
                    {
                        cv::Range ROI[2] = {{x*scaleFactorW, (x+1)*scaleFactorW},
                                            {y*scaleFactorH, (y+1)*scaleFactorH}};
                        cv::Mat texture = nextImage(ROI);
                        if(std::min(width, height) > 3)
                            cv::GaussianBlur(texture, texture, cv::Size(3,3), 0.2);
                    }
            } else {
                cv::pyrDown(nextImage, nextImage);
            }
            //QImage result = qimage_from_cvmat(nextImage);
*/

            int mult = std::pow(2,level);
            QImage result = QImage(QSize(width, height), imageGL.format());
            for(int x = 0; x < width; x++)
                for(int y = 0; y < height; y++)
                    result.setPixel(x,y,imageGL.pixel(x*mult, y*mult));
            glTexImage2D(GL_TEXTURE_2D, level, GL_RGBA, result.width(), result.height(),
                         0, GL_RGBA, GL_UNSIGNED_BYTE, result.bits());

            width /= 2;
            height /= 2;
            level++;
        }
        /*
        glGenerateMipmap(GL_TEXTURE_2D);
        */

    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    int errorCode = glGetError();
    if(errorCode != 0){
        qDebug() << "error loading texture";
        return false;
    }

    textures[textureAtlasName] = textureID;

    return true;
}
