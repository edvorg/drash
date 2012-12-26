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

#ifndef DEBUGDRAWSYSTEM_H
#define DEBUGDRAWSYSTEM_H

#include "subsystem/subsystem.h"
#include <vector>
#include "../misc/matrix4.h"

namespace drash
{

class CCameraParams;
class CCamera;
class CPlane;
class CFigure;

class CDebugDrawSystem : public CSubsystem
{
public:
    virtual bool Init() override;
    virtual void Step(double _dt) override;
    virtual void Release() override;

    /// if _set_active is true, calls SetActiveCam() with created cam as param
    CCamera *CreateCam(const CCameraParams &_params, bool _set_active = false);
    void DestroyCam(CCamera *_cam);

    /// activates cam. Draw() will render objects, using parameter of activated cam
    void SetActiveCam(CCamera *_cam);

    /// returns acticvated cam
    inline CCamera *GetActiveCam() const;

    /// aspect ratio is window width in pixels divided on widow height
    /// you should use correct value for using coordinates conversion methods
    void SetAspectRatio(float _ratio);

    /// aspect ratio is window width in pixels divided on widow height
    inline float GetAspectRatio() const;

    void CastRay(const CVec2f &_pos, const CPlane &_plane, CVec3f &_result) const;

    /// finds objects, visible at specified postion in screen space coordinates
    /// returns nearest one
    CFigure *FindFigure(const CVec2f &_pos) const;

	/// draws connected CScene instance's objects
    void Draw() const;

    /// draws triangle giving screen space coordinates (-0.5,-0.5)..(0.5, 0.5) and color
    void DrawTriangle(const CVec2f &_p1, const CVec2f &_p2, const CVec2f &_p3, const b2Color &_col) const;

    /// draws line giving world space coordinates and color
    void DrawTriangle(const CVec3f &_p1, const CVec3f &_p2, const CVec3f &_p3, const b2Color &_col) const;

    /// draws line giving screen space coordinates (-0.5,-0.5)..(0.5, 0.5) start and end points
    /// and color
    void DrawLine(const CVec2f &_p1, const CVec2f &_p2, const b2Color &_col) const;

    /// draws line giving world space coordinates start and end points
    /// and color
    void DrawLine(const CVec3f &_p1, const CVec3f &_p2, const b2Color &_col) const;

    /// draws point giving screen space coordinates (-0.5,-0.5)..(0.5, 0.5)
    void DrawPoint(const CVec2f &_p, float _size, const b2Color &_col) const;

    /// draws point giving world space coordinates
    void DrawPoint(const CVec3f &_p, float _size, const b2Color &_col) const;

protected:
private:
    CCamera *mActiveCam = nullptr;
    std::vector<CCamera*> mCameras;
    float mAspectRatio = 1;
    CMatrix4f mViewMatrixTransposed;
};

inline CCamera *CDebugDrawSystem::GetActiveCam() const
{
    return mActiveCam;
}

inline float CDebugDrawSystem::GetAspectRatio() const
{
    return mAspectRatio;
}

}// namespace drash

#endif // DEBUGDRAWSYSTEM_H
