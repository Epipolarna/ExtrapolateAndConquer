#include "RenderSystem.hpp"

//texture variable names in shaders
const std::string RenderSystem::textureVariableNames[]= {std::string("tex0"),std::string("tex1"),std::string("tex2"),
														 std::string("tex3"),std::string("tex4"),std::string("tex5"),
														 std::string("tex6"),std::string("tex7"),std::string("tex8"),
														 std::string("tex9"),std::string("tex10"),std::string("tex11"),
														 std::string("tex12"),std::string("tex13"),std::string("tex14"),
														 std::string("tex15")};

RenderSystem::RenderSystem(){
	camera = NULL;
	worldData = NULL;
	resources = NULL;
}

void RenderSystem::processStep(Drawable& object){

	SpherePhysics& physics = getEntity(object).get<SpherePhysics>();

	//update the model matrix data...
	object.mMatrix.setToIdentity();
	object.mMatrix.translate(physics.position);
	object.mMatrix.rotate(physics.rotation2);
	object.mMatrix.scale(object.scale);	

	objectList.push_back(object);
}

void RenderSystem::batch(void){
	//TODO possibly some culling or something, possibly nothing at all
}

void RenderSystem::draw(void){
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	updateLighting();
	drawWorld();
	drawShadows();
	drawObjects();
}

void RenderSystem::updateLighting(void){
	sun.position = QVector3D(0,1.0,0);
}

void RenderSystem::setup(void){
	printf("setting upp render system \n");
	initializeOpenGLFunctions();
	if(camera == NULL){
		printf("Camera not assigned to renderer, crashing! \n");
		exit(0);
	}

	if(worldData == NULL){
		printf("World not assigned to renderer, crashing! \n");
		exit(0);
	}

	if(resources == NULL){
		printf("Resource manager not assigned to renderer, crashing!\n");
		exit(0);
	}
	resources->loadShader("oceanShader");
	resources->loadShader("phongTex");
	resources->loadShader("skyboxShader");
	resources->loadShader("terrainShader");
	resources->loadShader("depth");
	resources->loadShader("phong");

	objectShader = resources->getShader("phong");
	shadowShader = resources->getShader("phong");
	skyShader = resources->getShader("phong");
	waterShader = resources->getShader("phong");
	terrainShader = resources->getShader("phong");
}

//setters
void RenderSystem::setWorld(World* w){
	this->worldData = w;
}

void RenderSystem::setCamera(Camera* c){
	this->camera = c;
}

void RenderSystem::setResources(ResourceManager* rm){
	this->resources = rm;
}

//private methods...

void RenderSystem::drawWorld(void){
	//draw the skybox?

	//TODO allow for more than one single world chunk
	Model* worldModel = worldData->getChunks();
	currentProgram = terrainShader;

	terrainShader->bind();

    //just pass a unit matrix as world model data...
    QMatrix4x4 unitmatrix;
    unitmatrix.setToIdentity();

    setMatrices(unitmatrix);
    drawModel(worldModel);

	terrainShader->release();
	//draw the ocean?
}

void RenderSystem::drawShadows(void){
	shadowShader->bind();

	shadowShader->release();
}

void RenderSystem::drawObjects(void){

	//set the shader used for object drawing..
	objectShader->bind();

	//draw all objects in the world
	for(Drawable& d : objectList){
		drawObject(d);
	}
	objectShader->release();
}

void RenderSystem::setMaterial(MaterialParameters pm){
	currentProgram->setUniformValue("ambientCoeff", pm.ambientCoeff);
    currentProgram->setUniformValue("diffuseCoeff", pm.diffuseCoeff);
    currentProgram->setUniformValue("specularCoeff", pm.speculaCoeff);
    currentProgram->setUniformValue("specularExponent", pm.specularExponent);
}

void RenderSystem::setTexture(TextureParameters tp){

	for(int i=0; i < 16; ++i){
		//0 is not a valid texture ID, so just return
		if(tp.texture[i] == 0){
			return;
		}
		currentProgram->setUniformValue("tex0",tp.texture[i]);
		//might be wrong, need to double check...
		glActiveTexture(i);
		glBindTexture(GL_TEXTURE_2D,tp.texture[i]);
	}
	//only use one texture scale currently, so just set it to the first one
	currentProgram->setUniformValue("texScaling",tp.scale[0]);
}

void RenderSystem::setMatrices(QMatrix4x4 mMatrix){
	currentProgram->setUniformValue("mvpMatrix", camera->pMatrix*camera->vMatrix*mMatrix);
    currentProgram->setUniformValue("mMatrix", mMatrix);
    currentProgram->setUniformValue("vMatrix", camera->vMatrix);
    currentProgram->setUniformValue("pMatrix", camera->pMatrix);
}

void RenderSystem::drawModel(Model* modelData){
	modelData->VAO.bind();
    
	modelData->VBO.bind();
    currentProgram->enableAttributeArray("vertex");
    currentProgram->setAttributeBuffer("vertex",GL_FLOAT,0,3);
    
    modelData->NBO.bind();
    currentProgram->enableAttributeArray("normal");
    currentProgram->setAttributeBuffer("normal",GL_FLOAT,0,3);
    
	modelData->TBO.bind();
    currentProgram->enableAttributeArray("texCoord");
    currentProgram->setAttributeBuffer("texCoord",GL_FLOAT,0,2);
    
    glDrawElements(GL_TRIANGLES,modelData->index.size(),GL_UNSIGNED_INT,0L);	
}

void RenderSystem::drawObject(Drawable& object){
    
    setTexture(object.textureData);
    setMaterial(object.materialData);
    setMatrices(object.mMatrix);

	drawModel(object.modelData);

    object.modelData->VAO.release();
}
