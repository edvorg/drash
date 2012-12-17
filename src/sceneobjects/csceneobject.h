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

#pragma once
#ifndef CSCENEOBJECT_H
#define CSCENEOBJECT_H

#include <Box2D/Box2D.h>
#include "figure.h"
#include "../misc/animatedparam.h"
#include "../misc/vec3.h"

namespace drash
{

class CScene;
class CSceneObject;
class CExplosionParams;

class CSceneObjectGeometry
{
public:
    std::vector<CFigureParams> mFigures;
    std::vector<std::vector<unsigned int>> mDestructionGraph;

protected:
private:
};

class CSceneObjectParams
{
public:
    bool mDynamic = true;
    CVec3f mPos;
    float mAngle = 0;
    bool mFixedRotation = false;
};

class CSceneObject
{
public:
    friend class CPhysObserver;
    friend class CScene;
    friend class CExplosion;
    friend class CDebugDrawSystem;

    static const unsigned int mFiguresCountLimit = 16;

    typedef CSceneObjectGeometry GeometryT;
    typedef CSceneObjectParams ParamsT;

    inline CScene *GetScene();
    inline const CScene *GetScene() const;

    CFigure *CreateFigure(const CFigureParams &_params);
    void DestroyFigure(CFigure *_figure);
    inline CFigure * const *GetFigures() const;
    inline unsigned int EnumFigures() const;

    inline void SetDynamic(bool _dynamic);
    inline bool IsDynamic() const;
    inline void ApplyLinearImpulse( const CVec2 &_dir, const CVec2 &_pos );
    inline void SetLinearVelocity(const CVec2 &_vel);
    inline CVec2 GetLinearVelocity() const;
    inline void SetAngularVelocity(float _vel);
    inline float GetAngularVelocity() const;
    inline void SetFixedRotation( bool _fixed );
    inline void SetActive( bool _active );
    inline CVec2 GetWorldPoint(const CVec2 &_local_point) const;

    inline CAnimatedParam<CVec3f> &GetPos();
    inline CAnimatedParam<float> &GetAngle();

    virtual void ComputeBoundingBox();
    inline const b2AABB &GetBoundingBox() const;

    friend CLogger &operator <<(CLogger &_logger, const CSceneObject &_object);

    CSceneObjectGeometry* GetGeometry() const;
protected:
    CSceneObject(void);
    virtual ~CSceneObject(void);

    bool Init( const GeometryT &_geometry, const ParamsT &_params );
    virtual void Release(void);

    virtual void Step(double _dt);

    virtual void OnContactBegin(const CFigure *, const CFigure *);
    virtual void OnContactPreSolve(const CFigure *, const CFigure *);
    virtual void OnContactEnd(const CFigure *, const CFigure *);
    virtual void OnBoom( const CExplosionParams &_boom );
    virtual void DrawDebug() const;

    b2AABB mBoundingBox;

private:
    b2Body* mBody = nullptr;
    CScene* mScene = nullptr;

    //////////////////////////////////////////////////////
    /// values, changed only be owning system (CScene) ///

    /// if flag is true, object will be destoyed with next CScene::Step() invokation
    bool mDead = false;

    /// id. just for perfomance
    int mInternalId = -1;

    //////////////////////////////////////////////
    /// figures factory //////////////////////////

    CFigure *mFigures[mFiguresCountLimit];
    unsigned int mFiguresCount = 0;

    /// world space postition in physics world
    CAnimatedParam<CVec3f> mPos;

    /// rotation angle in radians
    CAnimatedParam<float> mAngle;

    /// color, used for debug rendering. will be removed
    float mColor[3];
};

inline CScene *CSceneObject::GetScene()
{
    return mScene;
}

inline const CScene *CSceneObject::GetScene() const
{
    return mScene;
}

inline void CSceneObject::SetDynamic( bool _dynamic )
{
    mBody->SetType( _dynamic ? b2_dynamicBody : b2_kinematicBody );
}

inline bool CSceneObject::IsDynamic() const
{
    return mBody->GetType() == b2_dynamicBody ? true : false;
}

inline CFigure * const *CSceneObject::GetFigures() const
{
    return mFigures;
}

inline unsigned int CSceneObject::EnumFigures() const
{
    return mFiguresCount;
}

inline void CSceneObject::ApplyLinearImpulse( const CVec2 &_dir, const CVec2 &_pos )
{
    mBody->ApplyLinearImpulse( _dir, _pos );
}

inline void CSceneObject::SetLinearVelocity(const CVec2 &_vel)
{
    mBody->SetLinearVelocity(_vel);
}

inline CVec2 CSceneObject::GetLinearVelocity() const
{
    return mBody->GetLinearVelocity();
}

inline void CSceneObject::SetAngularVelocity(float _vel)
{
    mBody->SetAngularVelocity(_vel);
}

float CSceneObject::GetAngularVelocity() const
{
    return mBody->GetAngularVelocity();
}

inline void CSceneObject::SetFixedRotation( bool _fixed )
{
    mBody->SetFixedRotation(_fixed);
}

inline void CSceneObject::SetActive( bool _active )
{
    mBody->SetActive(_active);
}

CVec2 CSceneObject::GetWorldPoint(const CVec2 &_local_point) const
{
    return mBody->GetWorldPoint(_local_point);
}

inline CAnimatedParam<CVec3f> &CSceneObject::GetPos()
{
    return mPos;
}

inline CAnimatedParam<float> &CSceneObject::GetAngle()
{
    return mAngle;
}

inline const b2AABB &CSceneObject::GetBoundingBox() const
{
    return mBoundingBox;
}

} // namespace drash

#endif // CSCENEOBJECT_H
