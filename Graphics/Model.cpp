#include "Model.hpp"

namespace graphics {

Model::Model()
{
    VBO = QOpenGLBuffer(QOpenGLBuffer::Type::VertexBuffer);
    IBO = QOpenGLBuffer(QOpenGLBuffer::Type::IndexBuffer);

    VBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    TBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    NBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);
    IBO.setUsagePattern(QOpenGLBuffer::UsagePattern::StaticDraw);

    mMatrix.setToIdentity();
}

Model::Model(QString objectFile){
    load(objectFile);
    uploadToGPU();
    mMatrix.setToIdentity();
}

void Model::load(QString filename) {
    model = glmReadOBJ(filename.toLatin1().data());
    if(model->numtexcoords < 1) {
        qWarning() << "Missing UV map.";
    }
    qWarning() << "Groups: " << model->numgroups;
    qWarning() << "Vertices: " << model->numvertices;
    qWarning() << "Triangles: " << model->numtriangles;
    qWarning() << "Normals: " << model->numnormals;

    GLMgroup* group;
    group = model->groups;
    while (group) {
        ModelGroup grp;
        for(int i = 0; i < group->numtriangles; i++) {
            for(int j = 0; j < 3; j++) {
                QVector3D vector(model->vertices[3 * model->triangles[group->triangles[i]].vindices[j] + 0],
                                 model->vertices[3 * model->triangles[group->triangles[i]].vindices[j] + 1],
                                 model->vertices[3 * model->triangles[group->triangles[i]].vindices[j] + 2]);
                grp.vertices.push_back(vector);
            }
            for(int j = 0; j < 3; j++) {
                QVector3D vector(model->normals[3 * model->triangles[group->triangles[i]].nindices[j] + 0],
                                 model->normals[3 * model->triangles[group->triangles[i]].nindices[j] + 1],
                                 model->normals[3 * model->triangles[group->triangles[i]].nindices[j] + 2]);
                grp.normals.push_back(vector);
            }
            if(model->numtexcoords > 0) {
                for(int j = 0; j < 3; j++) {
                    QVector3D vector(model->texcoords[2 * model->triangles[group->triangles[i]].tindices[j] + 0],
                                     model->texcoords[2 * model->triangles[group->triangles[i]].tindices[j] + 1],
                                     model->texcoords[2 * model->triangles[group->triangles[i]].tindices[j] + 2]);
                    grp.texCoords.push_back(vector);
                }
            }
        }
        groups.push_back(grp);
        group = group->next;
    }
    qDebug() << "loading file";
}

void Model::uploadToGPU()
{
    VAO.create();
    VAO.bind();

    if (groups.size() > 0){
        //for(int i = 0; i < groups.size(); i++){
        for(int i = 0; i < 1; i++){
            if(groups[i].vertices.size() > 0){
                VBO.create();
                VBO.bind();
                VBO.allocate(groups[i].vertices.constData(), groups[i].vertices.size()*3*sizeof(GLfloat));
            }

            if(groups[i].indices.size() > 0){
                IBO.create();
                IBO.bind();
                IBO.allocate(groups[i].indices.constData(), groups[i].indices.size()*sizeof(GLuint));
            }

            if(groups[i].normals.size() > 0){
                NBO.create();
                NBO.bind();
                NBO.allocate(groups[i].normals.constData(), groups[i].normals.size()*3*sizeof(GLfloat));
            }

            if(groups[i].texCoords.size() > 0){
                TBO.create();
                TBO.bind();
                TBO.allocate(groups[i].texCoords.constData(), groups[i].texCoords.size()*3*sizeof(GLfloat));
            }
        }
    }

    VAO.release();
}

}

