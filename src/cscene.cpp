#include "cscene.h"
#include "clogger.h"

namespace drash
{

CScene::CScene(void):
    mWorld( b2Vec2( 0, 0 ) ),
    mInitialized(false),
    mCountPlayers(0)
{
}

CScene::~CScene(void)
{
    if ( mInitialized == true )
    {
        LOG_WARN( "CScene::~CScene(): warning Release() called from destructor" );
        Release();
    }
}

bool CScene::Init(const CSceneParams &_params)
{
    if ( mInitialized == true )
    {
        LOG_WARN( "CScene::Init(): already initialized" );
        return false;
    }

    mWorld.SetContactListener(&mContactListener);
    mWorld.SetAllowSleeping(true);
    mWorld.SetContinuousPhysics(false);
    mWorld.SetGravity(_params.mGravity);

    mInitialized = true;
    return true;
}

void CScene::Release(void)
{
    if ( mInitialized == false )
    {
        LOG_WARN( "CScene::Release(): already released" );
        return;
    }

    if ( mObjects.size() )
    {
        LOG_WARN( "CScene::Release(): "<<
                  (unsigned int)mObjects.size()<<
                  " object(s) haven't been destroyed. Autorelease" );

        while ( mObjects.size() )
        {
            DestroyObject< CSceneObject >( *mObjects.begin() );
        }
    }

    mInitialized = false;
}

void CScene::Step( double _dt )
{
    if ( mInitialized == false )
    {
        LOG_ERR( "CScene::Step(): this is not initialized" );
        return;
    }

    mWorld.Step( _dt, mVelocityIterations, mPositionIterations );

    for ( ObjectsT::iterator i=mObjects.begin(), i_e=mObjects.end(); i!=i_e; i++ )
    {
        (*i)->Step(_dt);
    }
}

void CScene::SetDebugRenderer( CDebugRenderer *_renderer )
{
    mWorld.SetDebugDraw(_renderer);
}

void CScene::Draw(void)
{
    if ( mInitialized == false )
    {
        LOG_ERR( "CScene::Step(): this is not initialized" );
        return;
    }

    mWorld.DrawDebugData();
}

void CScene::OnPlayerEvent(const CPlayer::PlayerEvent &_event, unsigned int _playerId)
{
    if (_playerId >= mCountPlayers){
        LOG_ERR("Player with id = " <<
                _playerId << " no exist");

        return;
    }
    //LOG_INFO("sdfdf");
    mPlayers[_playerId]->onEvent(_event);
}

void CScene::AddPlayer()
{
    if (mCountPlayers == mPlayersMaxAmount){
        LOG_ERR("Achieved maximum Amount of Players");
        return;
    }

    mPlayers[mCountPlayers++] = CreateObject<CPlayer>(CPlayerParams());

}

} // namespace drash
