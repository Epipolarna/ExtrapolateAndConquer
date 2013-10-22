#include "object.h"

Object::Object(Model* _model, QGLShaderProgram* _program)
{
    model = _model;
    program = _program;


}

void Object::draw(Player* player, QMatrix4x4 &pMatrix, GLuint tex){
    program->bind();
    model->initShaderProgram(*program);

    //glBindTexture(GL_TEXTURE_2D, tex);
    program->setUniformValue("color", QColor(33, 157, 157));

    model->draw(*program, player->vMatrix, pMatrix, tex);

    program->release();
}
