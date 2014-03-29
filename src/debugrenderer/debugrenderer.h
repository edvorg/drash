// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012-2014 Edward Knyshov, Yuriy Shatilin.

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

#ifndef DRASH_DEBUGRENDERER_H
#define DRASH_DEBUGRENDERER_H

#include <functional>
#include "../misc/vec2.h"

namespace greng
{

class CTexture;
class CCamera;
class CGrengSystemsSet;
class CPointLight;
class CSpotLight;
class CMesh;
class CShaderProgram;

}

namespace drash
{

class CScene;
class CFigure;
class CSceneObjectGeometry;
class CSceneObjectParams;
class CSceneObject;
class CLevelObjectDesc;
class CGeometryManager;

class CDebugRenderer final
{
public:
    inline void SetScene(CScene *_scene);
    inline void SetGeometryManager(CGeometryManager *_geometry_manager);
    inline void SetGrengSystems(greng::CGrengSystemsSet *_greng_systems);

    inline void SetCamera(greng::CCamera *_camera);
    inline greng::CCamera *GetCamera() const;
    inline void SetLight(greng::CPointLight *_light);
    inline greng::CPointLight *GetLight() const;
    inline void SetSpotLight(greng::CSpotLight *_light);

    inline void SetTexCoordsScale(float _scale);

    bool Init();
    void Release();
    void Render() const;

    void RenderObject(const CSceneObjectGeometry &_geometry, const CSceneObjectParams &_params);

    /// finds objects, visible at specified postion in screen space coordinates
    /// returns nearest one
    CFigure *FindFigure(const greng::CCamera *_camera, const CVec2f &_pos) const;
    CSceneObject * FindObject(const greng::CCamera *_camera, const CVec2f &_pos) const;

    CLevelObjectDesc * FindObject(const greng::CCamera * _camera,
                                  const CVec2f & _pos,
                                  std::function<CLevelObjectDesc * (unsigned int)> _object_getter,
                                  unsigned int _objects_count);

protected:
private:
    bool InitTextures();
    bool InitShaders();

    greng::CMesh* CreateMesh(const CVec2f *_vertices,
                             unsigned int _vertices_count,
                             float _z,
                             float _depth) const;

    CScene* mScene = nullptr;
    CGeometryManager * mGeometryManager = nullptr;
    greng::CGrengSystemsSet *mGrengSystems = nullptr;

    greng::CShaderProgram *mShaderProgram1 = nullptr;
    greng::CShaderProgram *mShaderProgram2 = nullptr;
    greng::CCamera *mCamera = nullptr;
    greng::CPointLight *mLight = nullptr;
    greng::CSpotLight *mSpotLight1 = nullptr;
    greng::CTexture *mTexture1Diffuse = nullptr;
    greng::CTexture *mTexture1Normal = nullptr;

    float mTexCoordsScale = 1.0 / 40.0;
};

inline void CDebugRenderer::SetScene(CScene *_scene)
{
    mScene = _scene;
}

inline void CDebugRenderer::SetGeometryManager(CGeometryManager *_geometry_manager)
{
    mGeometryManager = _geometry_manager;
}

inline void CDebugRenderer::SetGrengSystems(greng::CGrengSystemsSet *_greng_systems)
{
    mGrengSystems = _greng_systems;
}

inline void CDebugRenderer::SetCamera(greng::CCamera *_camera)
{
    mCamera = _camera;
}

inline greng::CCamera *CDebugRenderer::GetCamera() const
{
    return mCamera;
}

inline void CDebugRenderer::SetLight(greng::CPointLight *_light)
{
    mLight = _light;
}

inline greng::CPointLight *CDebugRenderer::GetLight() const
{
    return mLight;
}

inline void CDebugRenderer::SetSpotLight(greng::CSpotLight *_light)
{
    mSpotLight1 = _light;
}

inline void CDebugRenderer::SetTexCoordsScale(float _scale)
{
    mTexCoordsScale = _scale;
}

} // namespace drash

#endif // DRASH_DEBUGRENDERER_H