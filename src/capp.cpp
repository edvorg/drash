#include "capp.h"

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <Box2D/Box2D.h>
#include <math.h>
#include "clogger.h"
#include "cdebugrenderer.h"
#include "cscene.h"
#include "cobjectsolidbody.h"
#include "cobjectcirclebody.h"
#include <iostream>
namespace drash
{

CAppParams::CAppParams()
{
}

CApp::CApp():
    mInitialized(false),
    mScene(),
    mTimer(),
    mDebugRenderer(),
    mCamera(NULL)
{

}

bool CApp::init( const CAppParams &_params )
{

    if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        LOG_ERR( "SDL_Init failed" );
        return false;
    }

    SDL_WM_SetCaption( "Drash", NULL );

    if ( SDL_SetVideoMode( mWidth, mHeight, 32, SDL_HWSURFACE | SDL_OPENGL ) == NULL )
    {
        LOG_ERR( "SDL_SetVideoMode failed" );
        return false;
    }

    glViewport( 0, 0, mWidth, mHeight );

    CSceneParams params;
    params.mGravity.Set( 0.0f, -9.8f );

    if ( mScene.Init(params) == false )
    {
        return false;
    }

    CCameraParams p;
    mCamera = mScene.CreateObject< CObjectCamera >(p);

    if ( mCamera == NULL )
    {
        return false;
    }

    mDebugRenderer.SetFlags(0xffffffff);
    mDebugRenderer.ClearFlags(b2Draw::e_aabbBit);
    mDebugRenderer.SetCamera(mCamera);

    if ( mDebugRenderer.Init() == false )
    {
        LOG_WARN( "CScene::Init(): debug renderer init failed" );
    }
    else
    {
        mScene.SetDebugRenderer(&mDebugRenderer);
    }

    mInitialized = true;
    return true;
}

void CApp::Release()
{
    assert( mInitialized == true );

    mScene.Release();
    SDL_Quit();
}

void CApp::Run()
{
    assert( mInitialized == true );

    CVec2 ver[4];
    ver[0].Set(-100,-10);
    ver[1].Set(100,-10);
    ver[2].Set(100,10);
    ver[3].Set(-100,10);
    CSolidBodyParams params;
    params.mVertices = ver;
    params.mVerticesCount =4;
    params.mPos.Set(0,-50);
    params.mDynamic = false;
    mScene.CreateObject< CObjectSolidBody>(params);

    const unsigned int delta = 100;
    const unsigned int speed = 3;

    mCamera->SetTargetSpeed( CVec2(speed) );

    bool movexr = false;
    bool movexl = false;
    bool moveyu = false;
    bool moveyd = false;

    mTimer.Reset(true);

    for ( ;; )
    {
        bool go = true;
        SDL_Event event;

        while ( SDL_PollEvent(&event) )
        {
            // Catch all events
            if ( event.type == SDL_MOUSEBUTTONDOWN ||
                 event.type == SDL_QUIT )
            {
                if ( event.button.button == SDL_BUTTON_LEFT )
                {
                    mTimer.SetPaused( !mTimer.IsPaused() );
                }
                else if ( event.button.button == SDL_BUTTON_WHEELDOWN )
                {
                    mCamera->SetZoom( mCamera->GetZoom() + 0.5f); //* mTimer.GetDeltaTime() );
                }
                else if ( event.button.button == SDL_BUTTON_WHEELUP )
                {
                    mCamera->SetZoom( mCamera->GetZoom() - 0.5f); //* mTimer.GetDeltaTime() );
                }
                else
                {
                    go = false;
                    break;
                }
            }
            else if ( event.type == SDL_KEYDOWN )
            {
                if ( event.key.keysym.sym == SDLK_RIGHT )
                {
                    movexr = true;
                    movexl = false;
                }
                else if ( event.key.keysym.sym == SDLK_LEFT )
                {
                    movexr = false;
                    movexl = true;
                }
                else if ( event.key.keysym.sym == SDLK_UP )
                {
                    moveyu = true;
                    moveyd = false;
                }
                else if ( event.key.keysym.sym == SDLK_DOWN )
                {
                    moveyu = false;
                    moveyd = true;
                }
            }
            else if ( event.type == SDL_KEYUP )
            {
                if ( event.key.keysym.sym == SDLK_RIGHT )
                {
                    movexr = false;
                }
                else if ( event.key.keysym.sym == SDLK_LEFT )
                {
                    movexl = false;
                }
                else if ( event.key.keysym.sym == SDLK_UP )
                {
                    moveyu = false;
                }
                else if ( event.key.keysym.sym == SDLK_DOWN )
                {
                    moveyd = false;
                }
            }
        }

        CVec2 newt = mCamera->GetPos();

        if ( movexr )
        {
            newt.x += delta;
            mCamera->SetTarget( newt );
        }
        else if ( movexl )
        {
            newt.x -= delta;
            mCamera->SetTarget( newt );
        }

        if ( moveyu )
        {
            newt.y += delta;
            mCamera->SetTarget( newt );
        }
        else if ( moveyd )
        {
            newt.y -= delta;
            mCamera->SetTarget( newt );
        }

        // If event for exit
        if (!go)
        {
            break;
        }

        Update();
        Render();
    }
}

void CApp::Update()
{
    mTimer.Tick();
    mScene.Step( mTimer.GetDeltaTime() );

    CSolidBodyParams par;
    par.mPos.Rand(-100,100);
    mScene.CreateObject< CObjectSolidBody >(par);
}

void CApp::Render()
{
    glClearColor( 0.5f, 0.5f, 0.5f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    mScene.Draw();

    SDL_GL_SwapBuffers();
}

}// namespace drash
