#include "csceneobject.h"
#include "../diag/clogger.h"
#include "explosion.h"
#include <GL/gl.h>
#include "../misc/graphics.h"

namespace drash
{

CSceneObjectParams::CSceneObjectParams():
    mDynamic(true),
    mPos(0),
    mAngle(0),
    mFixedRotation(false)
{
}

CFigureParams::CFigureParams():
    mFriction(1),
    mRestitution(0),
    mMass(1),
    mVertices()
{
}

CSceneObject::CSceneObject(void):
    mBody(NULL),
    mScene(NULL),
    mDead(false),
    mInternalId(-1)
{
    mColor[0] = Randf( 0.15f, 0.8f, 0.01f );
    mColor[1] = Randf( 0.15f, 0.8f, 0.01f );
    mColor[2] = Randf( 0.15f, 0.8f, 0.01f );
}

CSceneObject::~CSceneObject(void)
{
    Release();
}

bool CSceneObject::Init( const CSceneObject::ParamsT &_params )
{
    if ( mBody == NULL )
    {
        return false;
    }

    mPos.Set(_params.mPos);
    mAngle.Set(_params.mAngle);

    mBody->SetTransform( _params.mPos, _params.mAngle );
    mBody->SetActive(true);
    mBody->SetAwake(true);
    mBody->SetSleepingAllowed(true);
    mBody->SetUserData(this);
    mBody->SetAngularDamping(0);
    mBody->SetBullet(false);
    mBody->SetFixedRotation( _params.mFixedRotation );
    mBody->SetLinearDamping(0);
    mBody->SetGravityScale(1.0f);
    mBody->SetType( _params.mDynamic ?
                    b2_dynamicBody :
                    b2_kinematicBody );

    for ( auto i = _params.mFigures.begin(), i_e = _params.mFigures.end(); i != i_e; i++ )
    {
        CreateFigure(*i);
    }

    return true;
}

void CSceneObject::Release()
{
    while ( b2Fixture *f = mBody->GetFixtureList() )
    {
        delete reinterpret_cast<CInterval*>( f->GetUserData() );
        f->SetUserData(NULL);
        mBody->DestroyFixture(f);
    }
}

void CSceneObject::Step( double _dt )
{
    if ( mBody->GetType() == b2_kinematicBody )
    {
        if ( mPos.IsTargetSet() )
        {
            mPos.Step(_dt);
            CVec2 lv = mPos.GetTarget();
            lv -= mPos.Get();
            lv /= mPos.GetTimeRemains();
            mBody->SetLinearVelocity(lv);
        }
        else
        {
            mPos.Set( mBody->GetWorldCenter() );
        }

        if ( mAngle.IsTargetSet() )
        {
            mAngle.Step(_dt);
            float av = ( mAngle.GetTarget() - mAngle.Get() ) / mAngle.GetTimeRemains();
            mBody->SetAngularVelocity(av);
        }
        else
        {
            mAngle.Set( mBody->GetAngle() );
        }
    }
    else if ( mBody->GetType() == b2_dynamicBody )
    {
        mPos.Set( mBody->GetWorldCenter() );
        mAngle.Set( mBody->GetAngle() );
    }
}

void CSceneObject::OnContactBegin( const CContact &_contact )
{
}

void CSceneObject::OnContactPreSolve( const CContact &_contact )
{
}

void CSceneObject::OnContactEnd( const CContact &_contact )
{
}

void CSceneObject::OnBoom( const CExplosionParams &_boom )
{
    CVec2 dir( GetBody()->GetWorldCenter() );
    dir -= _boom.mPos;

    float k = std::min( dir.Length(), _boom.mStregth )/ _boom.mStregth;

    dir.Normalize();
    dir *= k * _boom.mStregth;

    ApplyLinearImpulse( dir, mPos.Get() );
}

void CSceneObject::DrawDebug() const
{
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    unsigned int j = 0;
    for ( b2Fixture *f = mBody->GetFixtureList(); f != NULL; f = f->GetNext() )
    {
        if ( f->GetShape()->GetType() == b2Shape::e_polygon )
        {
            auto s = reinterpret_cast<b2PolygonShape*>( f->GetShape() );

            if (s)
            {
                b2Color diffuse( mColor[0], mColor[1], mColor[2] );
                const CInterval interval = f->GetUserData() ?
                                            *((CInterval*)f->GetUserData()) :
                                            CInterval(-1, 1);

                DrawBody(s->m_vertices, s->m_count, interval, diffuse);
            }
        }
        j++;
    }
}

const b2Body *CSceneObject::GetBody() const
{
    return mBody;
}

CScene *CSceneObject::GetScene()
{
    return mScene;
}

const CScene *CSceneObject::GetScene() const
{
    return mScene;
}

void CSceneObject::SetDead()
{
    mDead = true;
}

bool CSceneObject::IsDead() const
{
    return mDead;
}

void CSceneObject::SetDynamic( bool _dynamic )
{
    mBody->SetType( _dynamic ? b2_dynamicBody : b2_kinematicBody );
}

void CSceneObject::CreateFigure( const CFigureParams &_params )
{
    b2PolygonShape s;

    if ( _params.mVertices.size() == 0 )
    {
        s.SetAsBox( 1.0f, 1.0f );
    }
    else
    {
        s.Set( &*_params.mVertices.begin(), _params.mVertices.size() );
    }

    b2MassData md;
    s.ComputeMass( &md, 1.0 );

    b2FixtureDef fdef;
    fdef.density = _params.mMass / md.mass;
    fdef.friction = _params.mFriction;
    fdef.isSensor = false;
    fdef.restitution = _params.mRestitution;
    fdef.shape = &s;
    fdef.userData = NULL;

    b2Fixture *f = mBody->CreateFixture(&fdef);
    f->SetUserData( new CInterval( _params.mLayers ) );
}

void CSceneObject::ApplyLinearImpulse( const CVec2 &_dir, const CVec2 &_pos )
{
    mBody->ApplyLinearImpulse( _dir, _pos, true );
}

void CSceneObject::SetLinearVelocity( const CVec2 &_vel )
{
    mBody->SetLinearVelocity(_vel);
}

void CSceneObject::SetAngularVelocity( float _vel )
{
    mBody->SetAngularVelocity(_vel);
}

void CSceneObject::SetFixedRotation( bool _fixed )
{
    mBody->SetFixedRotation(_fixed);
}

void CSceneObject::SetActive( bool _active )
{
    mBody->SetActive(_active);
}

void CSceneObject::SetPos(const CVec2 &_pos)
{
    mBody->SetTransform( _pos, mBody->GetAngle() );
    mPos.Set(_pos);
}

void CSceneObject::SetPosTarget( const CVec2 &_target, double _time, const AnimationBehavior &_behavior )
{
    mPos.SetTarget( _target, _time, _behavior );
}

const CAnimatedParam<CVec2> &CSceneObject::GetPos() const
{
    return mPos;
}

void CSceneObject::SetAngle(float _angle)
{
    mBody->SetTransform( mBody->GetWorldCenter(), _angle );
    mAngle.Set(_angle);
}

void CSceneObject::SetAngleTarget( float _target, double _time, const AnimationBehavior &_behavior )
{
    mAngle.SetTarget( _target, _time, _behavior );
}

const CAnimatedParam<float> &CSceneObject::GetAngle() const
{
    return mAngle;
}

} // namespace drash
