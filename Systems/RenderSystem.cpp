#include "RenderSystem.hpp"

//texture variable names in shaders
const std::string RenderSystem::textureVariableNames[]= {std::string("tex0"),std::string("tex1"),std::string("tex2"),
														 std::string("tex3"),std::string("tex4"),std::string("tex5"),
														 std::string("tex6"),std::string("tex7"),std::string("tex8"),
														 std::string("tex9"),std::string("tex10"),std::string("tex11"),
														 std::string("tex12"),std::string("tex13"),std::string("tex14"),
														 std::string("tex15")};

void RenderSystem::processStep(Drawable& object){

	//TODO compute model matrix
	objectList.push_back(object);
}

void RenderSystem::batch(void){
	drawWorld();
	drawShadows();
	drawObjects();
}

void RenderSystem::setup(void){

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

}

void RenderSystem::drawShadows(void){

}

void RenderSystem::drawObjects(void){

	//set the shader used for object drawing..
	currentProgram = objectList[0].shader;
	//TODO implement support for different shaders...

	//draw all objects in the world
	for(Drawable& d : objectList){
		drawObject(d);
	}
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

void RenderSystem::drawObject(Drawable& object){
	//program->bind();
    
    setTexture(object.textureData);
    setMaterial(object.materialData);
    setMatrices(object.mMatrix);

	object.modelData->VAO.bind();
    
	object.modelData->VBO.bind();
    currentProgram->enableAttributeArray("vertex");
    currentProgram->setAttributeBuffer("vertex",GL_FLOAT,0,3);
    
    object.modelData->NBO.bind();
    currentProgram->enableAttributeArray("normal");
    currentProgram->setAttributeBuffer("normal",GL_FLOAT,0,3);
    
	object.modelData->TBO.bind();
    currentProgram->enableAttributeArray("texCoord");
    currentProgram->setAttributeBuffer("texCoord",GL_FLOAT,0,2);
    
    glDrawElements(GL_TRIANGLES,object.modelData->index.size(),GL_UNSIGNED_INT,0L);

    object.modelData->VAO.release();
    //program->release();
}
