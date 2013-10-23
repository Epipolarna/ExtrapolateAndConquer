#include "Object.hpp"
#include <QOpenGLFunctions>

namespace graphics {

Object::Object(Model *_model, QGLShaderProgram *_program)
{
    model = _model;
    program = _program;

    mMatrix.setToIdentity();

    ambientCoeff  = 0.2;
    diffuseCoeff  = 0.6;
    specularCoeff = 100;
    specularExponent = 50;

    if(program->isLinked()){
        vertexAttr = program->attributeLocation("vertex");
        normalAttr = program->attributeLocation("normal");
        texCoordAttr = program->attributeLocation("texCoord");
        textureUniform = program->uniformLocation("tex");
        mvpMatUniform = program->uniformLocation("mvpMatrix");
        mMatUniform = program->uniformLocation("mMatrix");
        vMatUniform = program->uniformLocation("vMatrix");
        pMatUniform = program->uniformLocation("pMatrix");
        ambientCoeffUniform = program->uniformLocation("ambientCoeff");
        diffuseCoeffUniform = program->uniformLocation("diffuseCoeff");
        specularCoeffUniform = program->uniformLocation("specularCoeff");
        specularExponentUniform = program->uniformLocation("specularExponent");
    } else {
        qWarning("Your shader is broken");
    }
}

void Object::draw(QMatrix4x4 &vMatrix, QMatrix4x4 &pMatrix)
{
    program->bind();

    program->setUniformValue(mvpMatUniform, pMatrix*vMatrix*mMatrix);
    program->setUniformValue(mMatUniform, mMatrix);
    program->setUniformValue(vMatUniform, vMatrix);
    program->setUniformValue(pMatUniform, pMatrix);

    program->enableAttributeArray(vertexAttr);
    program->setAttributeBuffer(vertexAttr, GL_FLOAT, 0, 3);

    model->VAO.bind();

    glDrawArrays(GL_TRIANGLES, 0, 9);


    /*
    program->bind();
    program->setUniformValue(mvpMatUniform, pMatrix*vMatrix*mMatrix);
    program->setUniformValue(mMatUniform, mMatrix);
    program->setUniformValue(vMatUniform, vMatrix);
    program->setUniformValue(pMatUniform, pMatrix);

    program->enableAttributeArray(normalAttr);
    program->enableAttributeArray(vertexAttr);
    program->enableAttributeArray(texCoordAttr);

    //glBindTexture(GL_TEXTURE_2D, texture);
    foreach(ModelGroup grp, model->groups) {
        foreach(ModelTriangle triangle, grp.triangles) {
            //program->setUniformValue(textureUniform, tex);    // use texture unit 0

            program->setAttributeArray(vertexAttr, triangle.vertices.constData());
            program->setAttributeArray(normalAttr, triangle.normals.constData());
            program->setAttributeArray(texCoordAttr, triangle.texcoords.constData());

            glDrawArrays(GL_TRIANGLES, 0, triangle.vertices.size());
        }
    }

    program->disableAttributeArray(normalAttr);
    program->disableAttributeArray(vertexAttr);
    program->disableAttributeArray(texCoordAttr);
    */

    program->release();
}

}

