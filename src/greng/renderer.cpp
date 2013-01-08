// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012 Edward Knyshov, Yuriy Shatilin.

This file is part of the drash GPL Source Code (drash Source Code).

drash Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

drash Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with drash Source Code.  If not, see <http://www.gnu.org/licenses/>.

*/
// DRASH_LICENSE_END

#include <GL/glew.h>
#include "renderer.h"
#include "mesh.h"
#include "texture.h"
#include "../misc/matrix4.h"
#include "shaderprogram.h"
#include "pointlight.h"

namespace greng
{

using drash::CLogger;

bool CRenderer::Init()
{
    return true;
}

void CRenderer::RenderMesh(const CMesh *_mesh,
                           unsigned int _submesh,
                           const CTexture *_texture,
                           const drash::CMatrix4f &_model_view)
{
    if (_submesh >= _mesh->mMaterialOffsets.size() - 1)
    {
        return;
    }

    drash::CMatrix4f m(_model_view);
    m.Transpose();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(m.mData);

    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glActiveTexture(GL_TEXTURE0);
    if (_texture == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _texture->mTextureBufferId);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _mesh->mVertexBufferId);
    glVertexPointer(3, GL_FLOAT, sizeof(CVertex), nullptr);
    glTexCoordPointer(2, GL_FLOAT, sizeof(CVertex), reinterpret_cast<GLvoid*>(sizeof(drash::CVec3f)));
    glColorPointer(4, GL_FLOAT, sizeof(CVertex), reinterpret_cast<GLvoid*>(sizeof(drash::CVec3f) +
                                                                              sizeof(drash::CVec2f)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->mIndexBufferId);
    glDrawElements(GL_TRIANGLES,
                   _mesh->mMaterialOffsets[_submesh+1] - _mesh->mMaterialOffsets[_submesh],
                   GL_UNSIGNED_INT,
                   reinterpret_cast<const GLvoid*>(sizeof(unsigned int) * _mesh->mMaterialOffsets[_submesh]));

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

void CRenderer::RenderMesh(const CMesh *_mesh,
                           unsigned int _submesh,
                           const CTexture *_texture,
                           const CShaderProgram *_program,
                           const drash::CMatrix4f &_model,
                           const drash::CMatrix4f &_view,
                           const drash::CMatrix4f &_model_view,
                           const drash::CMatrix4f &_proj_matrix,
                           const CPointLight *_light)
{
    if (_submesh >= _mesh->mMaterialOffsets.size() - 1)
    {
        return;
    }

    //glCullFace(GL_BACK);
    //glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_TEXTURE_2D);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    if (_texture == nullptr)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, _texture->mTextureBufferId);
    }

    glBindBuffer(GL_ARRAY_BUFFER, _mesh->mVertexBufferId);
    glVertexPointer(3, GL_FLOAT, sizeof(CVertex), nullptr);
    glTexCoordPointer(2, GL_FLOAT, sizeof(CVertex), reinterpret_cast<GLvoid*>(sizeof(drash::CVec3f)));
    glNormalPointer(GL_FLOAT, sizeof(CVertex), reinterpret_cast<GLvoid*>(sizeof(drash::CVec3f) +
                                                                         sizeof(drash::CVec2f)));
    glColorPointer(4, GL_FLOAT, sizeof(CVertex), reinterpret_cast<GLvoid*>(sizeof(drash::CVec3f) +
                                                                           sizeof(drash::CVec2f) +
                                                                           sizeof(drash::CVec3f)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _mesh->mIndexBufferId);

    glUseProgram(_program->mProgramId);

    int mloc = glGetUniformLocation(_program->mProgramId, "gModelMatrix");
    int vloc = glGetUniformLocation(_program->mProgramId, "gViewMatrix");
    int mvloc = glGetUniformLocation(_program->mProgramId, "gModelViewMatrix");
    int ploc = glGetUniformLocation(_program->mProgramId, "gProjMatrix");
    int t1loc = glGetUniformLocation(_program->mProgramId, "gTex1");
    int l1ploc = glGetUniformLocation(_program->mProgramId, "gLight1Position");

    if (mloc != -1)
    {
        glUniformMatrix4fv(mloc, 1, GL_TRUE, _model.mData);
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gModelMatrix attribute");
    }

    if (vloc != -1)
    {
        glUniformMatrix4fv(vloc, 1, GL_TRUE, _view.mData);
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gViewMatrix attribute");
    }

    if (mvloc != -1)
    {
        glUniformMatrix4fv(mvloc, 1, GL_TRUE, _model_view.mData);
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gModelViewMatrix attribute");
    }

    if (ploc != -1)
    {
        glUniformMatrix4fv(ploc, 1, GL_TRUE, _proj_matrix.mData);
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gProjMatrix attribute");
    }

    if (t1loc != -1)
    {
        glUniform1i(t1loc, 0);
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gTex1 attribute");
    }

    if (l1ploc != -1)
    {
        glUniform3fv(l1ploc, 1, reinterpret_cast<const GLfloat*>(&_light->mPosition));
    }
    else
    {
        LOG_ERR("CRenderer::RenderMesh(): Unable to find gLight1Position attribute");
    }

    glDrawElements(GL_TRIANGLES,
                   _mesh->mMaterialOffsets[_submesh+1] - _mesh->mMaterialOffsets[_submesh],
                   GL_UNSIGNED_INT,
                   reinterpret_cast<const GLvoid*>(sizeof(unsigned int) * _mesh->mMaterialOffsets[_submesh]));

    glUseProgram(0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);

    glDisable(GL_TEXTURE_2D);
}

} // namespace greng