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

#include "ctestapp3-drashtest1.h"

#include "../cscene.h"
#include "../sceneobjects.h"

namespace drash
{

void GenDrashBodyParams( CDrashBodyParams* _params , float _subset_size, unsigned int _level, unsigned int _levels );

bool CTestApp3::Init()
{
    if ( CApp::Init() == false )
    {
        return false;
    }

    CSceneObjectGeometry sbg;
    sbg.mFigures.resize(1);
    sbg.mFigures[0].mDepth = 40;
    sbg.mFigures[0].mRestitution = 0.0;
    sbg.mFigures[0].mVertices.push_back( CVec2( -300, -5 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 300, -5 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 300, 5 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( -300, 5 ) );
    CSceneObjectParams sbp;
    sbp.mDynamic = false;
    sbp.mAngle = 0;
    GetScene().CreateObject<CSceneObject>(sbg, sbp);

    sbp.mPos.Set( 0, 600 );
    GetScene().CreateObject<CSceneObject>(sbg, sbp);

    sbg.mFigures[0].mVertices.clear();
    sbg.mFigures.resize(1);
    sbg.mFigures[0].mVertices.push_back( CVec2( -5, -300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 5, -300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 5, 300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( -5, 300 ) );
    sbp.mPos.Set( -300, 300 );
    GetScene().CreateObject<CSceneObject>(sbg, sbp);

    sbg.mFigures[0].mVertices.clear();
    sbg.mFigures.resize(1);
    sbg.mFigures[0].mVertices.push_back( CVec2( -5, -300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 5, -300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( 5, 300 ) );
    sbg.mFigures[0].mVertices.push_back( CVec2( -5, 300 ) );
    sbp.mPos.Set( 300, 300 );
    GetScene().CreateObject<CSceneObject>(sbg, sbp);

    CDrashBodyTemplate *b1 = GetTemplateSystem().CreateDrashBodyTemplate("box_small");
    b1->mGeometry.mFigures.resize(1);
    b1->mGeometry.mFigures[0].mDepth = 10;
    b1->mGeometry.mFigures[0].mVertices.push_back( CVec2( -5, -5 ) );
    b1->mGeometry.mFigures[0].mVertices.push_back( CVec2( 5, -5 ) );
    b1->mGeometry.mFigures[0].mVertices.push_back( CVec2( 5, 5 ) );
    b1->mGeometry.mFigures[0].mVertices.push_back( CVec2( -5, 5 ) );
    CDrashBodyTemplate *b0 = GetTemplateSystem().CreateDrashBodyTemplate("box_big");
    b0->mGeometry.mFigures.resize(1);
    b0->mGeometry.mFigures[0].mDepth = 20;
    b0->mGeometry.mFigures[0].mVertices.push_back( CVec2( -10, -10 ) );
    b0->mGeometry.mFigures[0].mVertices.push_back( CVec2( 10, -10 ) );
    b0->mGeometry.mFigures[0].mVertices.push_back( CVec2( 10, 10 ) );
    b0->mGeometry.mFigures[0].mVertices.push_back( CVec2( -10, 10 ) );

    b0->mGeometry.mDestructionChilds.resize(8);
    b0->mGeometry.mDestructionChilds[0].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[0].mParams.mPos.Set(-5,-5);
    b0->mGeometry.mDestructionChilds[0].mParams.mZ = 5;
    b0->mGeometry.mDestructionChilds[1].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[1].mParams.mPos.Set(-5,5);
    b0->mGeometry.mDestructionChilds[1].mParams.mZ = 5;
    b0->mGeometry.mDestructionChilds[2].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[2].mParams.mPos.Set(5,5);
    b0->mGeometry.mDestructionChilds[2].mParams.mZ = 5;
    b0->mGeometry.mDestructionChilds[3].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[3].mParams.mPos.Set(5,-5);
    b0->mGeometry.mDestructionChilds[3].mParams.mZ = 5;
    b0->mGeometry.mDestructionChilds[4].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[4].mParams.mPos.Set(-5,-5);
    b0->mGeometry.mDestructionChilds[4].mParams.mZ = -5;
    b0->mGeometry.mDestructionChilds[5].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[5].mParams.mPos.Set(-5,5);
    b0->mGeometry.mDestructionChilds[5].mParams.mZ = -5;
    b0->mGeometry.mDestructionChilds[6].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[6].mParams.mPos.Set(5,5);
    b0->mGeometry.mDestructionChilds[6].mParams.mZ = -5;
    b0->mGeometry.mDestructionChilds[7].mTemplate = b1;
    b0->mGeometry.mDestructionChilds[7].mParams.mPos.Set(5,-5);
    b0->mGeometry.mDestructionChilds[7].mParams.mZ = -5;

    CDrashBodyParams dbp;
    dbp.mPos.Set( 0, 100 );
    dbp.mAngle = M_PI / 4;

    GetScene().CreateObject<CDrashBody>(b0->mGeometry, dbp);

    CSceneObjectGeometry pg;
    pg.mFigures.resize(1);
    pg.mFigures[0].mMass = 3;
    CSceneObjectParams pp;
    pp.mPos.Set( -200, 100 );
    GetScene().CreateObject<CSceneObject>(pg, pp)->SetLinearVelocity( CVec2( 200, 0 ) );

    CSceneObjectGeometry ppg;
    ppg.mFigures.resize(1);
    ppg.mFigures[0].mDepth = 2;
    CPlayerParams ppp;
    ppp.mPos.Set( 0, 10 );
    GetPlayersSystem().AddPlayer(ppg, ppp);

    CSceneObjectGeometry platform_geometry;
    platform_geometry.mFigures.resize(1);
    platform_geometry.mFigures[0].mDepth = 8;
    platform_geometry.mFigures[0].mFriction = 1.0;
    platform_geometry.mFigures[0].mVertices.push_back( CVec2( -100, -5 ) );
    platform_geometry.mFigures[0].mVertices.push_back( CVec2( 100, -5 ) );
    platform_geometry.mFigures[0].mVertices.push_back( CVec2( 100, 5 ) );
    platform_geometry.mFigures[0].mVertices.push_back( CVec2( -100, 5 ) );
    CSceneObject::ParamsT platform_params;
    platform_params.mPos.Set( 0, 50 );
    platform_params.mAngle = -M_PI / 18.0;
    platform_params.mDynamic = false;

    CSceneObject *platform = GetScene().CreateObject<CSceneObject>(platform_geometry, platform_params);
    platform->SetPos( CVec2( -100, 50 ) );
    platform->SetPosTarget( CVec2( 100, 50 ), 10, AnimationBehaviorBounce );
    platform->SetAngleTarget( M_PI / 18.0, 10, AnimationBehaviorBounce );

    GetDebugDrawSystem().GetActiveCam()->SetZTarget( 280, 1.0f, AnimationBehaviorSingle );

    return true;
}

void CTestApp3::Step(double _dt)
{
    CApp::Step(_dt);

    for (CAppEvent e=PopEvent(); e.GetType()!=EventUnknown; e=PopEvent())
    {
        if (e.GetType() == EventKeyboard)
        {
            switch (e.GetKey())
            {
            case EventKeyW:
                GetPlayersSystem().OnPlayerEvent( CPlayerEvent( CPlayerEvent::PlayerActionMoveDeep ), 0 );
                break;
            case EventKeyA:
                GetPlayersSystem().OnPlayerEvent( CPlayerEvent( CPlayerEvent::PlayerActionMoveLeft ), 0 );
                break;
            case EventKeyS:
                GetPlayersSystem().OnPlayerEvent( CPlayerEvent( CPlayerEvent::PlayerActionMoveOut ), 0 );
                break;
            case EventKeyD:
                GetPlayersSystem().OnPlayerEvent( CPlayerEvent( CPlayerEvent::PlayerActionMoveRight ), 0 );
                break;
            case EventKeySpace:
                GetPlayersSystem().OnPlayerEvent( CPlayerEvent( CPlayerEvent::PlayerActionJump ), 0 );
                break;
            default:
                break;
            }
        }
        else if (e.GetType() == EventMouse)
        {
            if ((e.GetButton() == EventButtonWheelUp ||
				 e.GetButton() == EventButtonWheelDown) &&
				 GetDebugDrawSystem().GetActiveCam() != nullptr)
            {
                float pos = GetDebugDrawSystem().GetActiveCam()->GetZ().GetTarget();
                pos += (e.GetButton() == EventButtonWheelUp ? 10.0f : -10.0f);
                GetDebugDrawSystem().GetActiveCam()->SetZTarget( pos, 0.3, AnimationBehaviorSingle );
            }
            else if (e.GetButton() == EventButtonLeft)
            {
                CSceneObjectGeometry g;
                CExplosionParams p;
                p.mLifeTime = 1;
                p.mStregth = -5;
                p.mRadius = 200;
                p.mPos = e.GetPos();
                auto cam = GetDebugDrawSystem().GetActiveCam();
                GetDebugDrawSystem().ScreenSpaceToWorldSpace(p.mPos, -cam->GetZ().Get());
                GetScene().CreateObject<CExplosion>(g, p);
			}
            else if (e.GetButton() == EventButtonRight)
            {
                static CSceneObject *o1 = nullptr;
                static CSceneObject *o2 = nullptr;

                if (o1 == nullptr)
                {
                    o1 = GetDebugDrawSystem().FindObject(e.GetPos());
                }
                else if (o2 == nullptr)
                {
                    o2 = GetDebugDrawSystem().FindObject(e.GetPos());
                    if (o1 == o2)
                    {
                        o2 = nullptr;
                    }
                    else if (o2 != nullptr)
                    {
                        GetScene().CreateJoint(o1, o2, o1->GetPos().Get());
                        o1 = nullptr;
                        o2 = nullptr;
                    }
                }
            }
        }
    }

    if (GetPlayersSystem().EnumPlayers())
    {
        CPlayer *p = GetPlayersSystem().GetPlayers()[0];
        GetDebugDrawSystem().GetActiveCam()->SetPosTarget( p->GetBody()->GetWorldCenter(), 1.0, AnimationBehaviorSingle );
    }
}

} // namespace drash

