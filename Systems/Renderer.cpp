#include "Renderer.hpp"

void RenderSystem::processStep(Drawable& object){

  std::map<QOpenglShaderProgram*,std::vector<Drawable>>::iterator pos = 
	  drawList.find(object.shader);

  if(pos == drawList.end()){
	std::vector<Drawable> newList;
	newList.push_back(object);
	drawList.emplace(newList);
  }else{
	*pos.push_back(object);
  }
}

void RenderSystem::batch(void){
	drawWorld();
	drawShadows();
	drawObjects();
}

void RenderSystem::setWorld(World* w){
	this->worldData = w;
}

void RenderSystem::setCamera(Camera* c){
	this->camera = c;
}

//private methods goes here...
void RenderSystem::drawWorld(void){

}

void RenderSystem::drawShadows(void){

}

void RenderSystem::drawObjects(void){

}
