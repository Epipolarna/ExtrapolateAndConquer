 #include "GraphicsWidget.hpp"

 GraphicsWidget::GraphicsWidget(QGLFormat format, QWidget *parent) : 
 	QGLWidget(format, parent)
 { 
    r = new Renderer();
 }

GraphicsWidget::~GraphicsWidget(){
	//TODO probably need to release something
}

Renderer* GraphicsWidget::getRenderer(void){
	return r;
}

void GraphicsWidget::initializeGL(){
	initializeOpenGLFunctions();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0.0f, 0.0f, 1.0f, 0);

	int errorCode = glGetError();
	if(errorCode != 0){
		printf("error after initing gl is: %x \n",errorCode);
	}
}

void GraphicsWidget::resizeGL(int width, int height){

    r->pMatrix.setToIdentity();
    r->pMatrix.perspective(60.0, (float) width / (float) height, 0.1, 1000);

    glViewport(0, 0, width, height);
}

void GraphicsWidget::paintGL(){
    qDebug() << "doing the repaint! ";
    r->repaint();
}
