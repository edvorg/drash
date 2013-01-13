// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012-2013 Edward Knyshov, Yuriy Shatilin.

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
#include "shaderprogram.h"
#include "pointlight.h"
#include "camera.h"

namespace greng
{

using drash::CLogger;

bool CRenderer::Init()
{
    return true;
}

void CRenderer::RenderMesh(const CMesh *_mesh,
                           unsigned int _submesh,
                           const CTexture * const *_textures,
                           unsigned int _textures_count,
                           const CShaderProgram *_program,
                           const drash::CMatrix4f *_model,
                           const drash::CMatrix4f *_view,
                           const drash::CMatrix4f *_model_view,
                           const drash::CMatrix4f *_proj_matrix,
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

    static const unsigned int textures_count_limit = 3;
    _textures_count = drash::math::Min<unsigned int>(_textures_count, textures_count_limit);

    if (_textures == nullptr)
    {
        for (unsigned int i = 0; i < _textures_count; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
    else
    {
        for (unsigned int i = 0; i < _textures_count; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, _textures[i]->mTextureBufferId);

            std::ostringstream os;
            os<<"gTex"<<i+1;

            int t1loc = glGetUniformLocation(_program->mProgramId, os.str().c_str());
            if (t1loc != -1)
            {
                glUniform1i(t1loc, i);
            }
            else
            {
                LOG_ERR("CRenderer::RenderMesh(): Unable to find "<<os.str().c_str()<<" attribute");
            }
        }
    }

    if (_model != nullptr)
    {
        int mloc = glGetUniformLocation(_program->mProgramId, "gModelMatrix");
        if (mloc != -1)
        {
            glUniformMatrix4fv(mloc, 1, GL_TRUE, _model->mData);
        }
        else
        {
            LOG_ERR("CRenderer::RenderMesh(): Unable to find gModelMatrix attribute");
        }
    }

    if (_view != nullptr)
    {
        int vloc = glGetUniformLocation(_program->mProgramId, "gViewMatrix");
        if (vloc != -1)
        {
            glUniformMatrix4fv(vloc, 1, GL_TRUE, _view->mData);
        }
        else
        {
            LOG_ERR("CRenderer::RenderMesh(): Unable to find gViewMatrix attribute");
        }
    }

    if (_model_view != nullptr)
    {
        int mvloc = glGetUniformLocation(_program->mProgramId, "gModelViewMatrix");
        if (mvloc != -1)
        {
            glUniformMatrix4fv(mvloc, 1, GL_TRUE, _model_view->mData);
        }
        else
        {
            LOG_ERR("CRenderer::RenderMesh(): Unable to find gModelViewMatrix attribute");
        }
    }

    if (_proj_matrix != nullptr)
    {
        int ploc = glGetUniformLocation(_program->mProgramId, "gProjMatrix");
        if (ploc != -1)
        {
            glUniformMatrix4fv(ploc, 1, GL_TRUE, _proj_matrix->mData);
        }
        else
        {
            LOG_ERR("CRenderer::RenderMesh(): Unable to find gProjMatrix attribute");
        }
    }

    if (_light != nullptr)
    {
        int l1ploc = glGetUniformLocation(_program->mProgramId, "gLight1Position");
        if (l1ploc != -1)
        {
            glUniform3fv(l1ploc, 1, reinterpret_cast<const GLfloat*>(&_light->mPosition));
        }
        else
        {
            LOG_ERR("CRenderer::RenderMesh(): Unable to find gLight1Position attribute");
        }
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

void CRenderer::DrawTriangle(const CVec2f &_p1,
                             const CVec2f &_p2,
                             const CVec2f &_p3,
                             const CColor4f &_col,
                             bool _depth_test) const
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, 1, -1);

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLES);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p1.mX, _p1.mY);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p2.mX, _p2.mY);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p3.mX, _p3.mY);
    glEnd();
}

void CRenderer::DrawTriangle(const CCamera *_camera, const CVec3f &_p1,
                             const CVec3f &_p2,
                             const CVec3f &_p3,
                             const CColor4f &_col,
                             bool _depth_test) const
{
    if (_camera == nullptr)
    {
        LOG_ERR("CRenderer::DrawTriangle(): _camera is not specified");
        return;
    }

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(_camera->GetViewMatrixTransposed().mData);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(_camera->GetProjectionMatrixTransposed().mData);

    glBegin(GL_TRIANGLES);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p1.mX, _p1.mY, _p1.mZ);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p2.mX, _p2.mY, _p2.mZ);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p3.mX, _p3.mY, _p3.mZ);
    glEnd();
}

void CRenderer::DrawLine(const CVec2f &_p1,
                         const CVec2f &_p2,
                         float _width,
                         const CColor4f &_col,
                         bool _depth_test) const
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, 1, -1);

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(_width);

    glBegin(GL_LINES);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p1.mX, _p1.mY);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p2.mX, _p2.mY);
    glEnd();
}

void CRenderer::DrawLine(const CCamera *_camera, const CVec3f &_p1,
                         const CVec3f &_p2,
                         float _width,
                         const CColor4f &_col,
                         bool _depth_test) const
{
    if (_camera == nullptr)
    {
        return;
    }

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(_camera->GetViewMatrixTransposed().mData);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(_camera->GetProjectionMatrixTransposed().mData);

    glLineWidth(_width);

    glBegin(GL_LINES);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p1.mX, _p1.mY, _p1.mZ);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p2.mX, _p2.mY, _p2.mZ);
    glEnd();
}

void CRenderer::DrawPoint(const CVec2f &_p,
                          float _size,
                          const CColor4f &_col,
                          bool _depth_test) const
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-0.5, 0.5, -0.5, 0.5, 1, -1);

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(_size);

    glBegin(GL_POINTS);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex2f(_p.mX, _p.mY);
    glEnd();
}

void CRenderer::DrawPoint(const CCamera *_camera,
                          const CVec3f &_p,
                          float _size,
                          const CColor4f &_col,
                          bool _depth_test) const
{
    if (_camera == nullptr)
    {
        return;
    }

    if (_depth_test == true)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(_camera->GetViewMatrixTransposed().mData);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(_camera->GetProjectionMatrixTransposed().mData);

    glPointSize(_size);

    glBegin(GL_POINTS);
    glColor4f(_col.mR, _col.mG, _col.mB, _col.mA);
    glVertex3f(_p.mX, _p.mY, _p.mZ);
    glEnd();
}

} // namespace greng
