#include "Model.hpp"

namespace graphics {

Model::Model()
{
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

    //for(int i = 0; i < groups.size(); i++){
    for(int i = 0; i < 1; i++){
    VBO.create();
    VBO.bind();
    VBO.allocate(groups[i].vertices.constData(), groups[i].vertices.size()*3*sizeof(GLfloat));

    NBO.create();
    NBO.bind();
    NBO.allocate(groups[i].normals.constData(), groups[i].normals.size()*3*sizeof(GLfloat));
    }

            /*
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glGenBuffers(1, &NBO);
    glGenBuffers(1, &TBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertexArray.size()*sizeof(cv::Vec3f), &vertexArray.front(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, groups[0].triangles[0].vertices.size()*sizeof(QVector3D),
            groups[0].triangles[0].vertices.constData(), GL_STATIC_DRAW);
/*
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    //glBufferData(GL_ARRAY_BUFFER, normalArray.size()*sizeof(cv::Vec3f), &normalArray.front(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, groups[0].triangles[0].normals.size()*sizeof(QVector3D),
            groups[0].triangles[0].normals.constData(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    //glBufferData(GL_ARRAY_BUFFER, texCoordArray.size()*sizeof(cv::Vec2f), &texCoordArray.front(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, groups[0].triangles[0].vertices.size()*sizeof(QVector3D),
            groups[0].triangles[0].vertices.constData(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexArray.size()*sizeof(cv::Vec3i), &indexArray.front(), GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, groups[0].triangles[0].vertices.size()*sizeof(QVector3D),
            groups[0].triangles[0].vertices.constData(), GL_STATIC_DRAW);
            */

    VAO.release();
}

}

