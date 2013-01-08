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

#include "app.h"
#include "../greng/camera.h"

namespace drash
{

CApp::CApp()
{
}

bool CApp::Init()
{
    mQuit = true;

    CSceneParams params;
    params.mGravity.Set( 0, -9.8 );

    if (mScene.Init(params) == false)
    {
        return false;
    }

    mExplosionSystem.SetScene(&mScene);
    mPlayersSystem.SetScene(&mScene);
    mTemplateSystem.SetScene(&mScene);
    mUISystem.SetRenderer(&mRenderer);

    if (mExplosionSystem.Init() == false ||
        mPlayersSystem.Init() == false ||
        mTemplateSystem.Init() == false ||
        mEventSystem.Init() == false ||
        mUISystem.Init() == false ||
        mRenderer.Init() == false ||
        mCameraManager.Init() == false)
    {
        return false;
    }

    greng::CVertexShader *vs = mVertexShaderManager.CreateShaderFromFile("shader2.120.vs");
    greng::CFragmentShader *fs = mFragmentShaderManager.CreateShaderFromFile("shader2.120.fs");
    greng::CShaderProgram *sp = mShaderProgramManager.CreateProgram(vs, fs);

    if (sp == nullptr)
    {
        return false;
    }

    mDebugRenderer.SetShaderProgram(sp);
    mDebugRenderer.SetMeshManager(&mMeshManager);
    mDebugRenderer.SetRenderer(&mRenderer);
    mDebugRenderer.SetScene(&mScene);
    mDebugRenderer.SetTextureManager(&mTextureManager);

    if (mDebugRenderer.Init() == false)
    {
        return false;
    }

    mQuit = false;

    return true;
}

void CApp::Step(double _dt)
{
    if (mQuit)
    {
        mQuitHandler();
        return;    
    }

    mCurrentTimeDelta = _dt;

    mCameraManager.Step(_dt);
    mEventSystem.Process();
    mScene.Step(_dt);
    mExplosionSystem.Step(_dt);
    mPlayersSystem.Step(_dt);
    mTemplateSystem.Step(_dt);
    mUISystem.Step(_dt);
}

void CApp::Release()
{
    mCameraManager.Release();
    mDebugRenderer.Release();
    mEventSystem.Release();
    mExplosionSystem.Release();
    mPlayersSystem.Release();
    mTemplateSystem.Release();
    mUISystem.Release();
    mScene.Release();

    mExplosionSystem.SetScene(nullptr);
    mPlayersSystem.SetScene(nullptr);
    mTemplateSystem.SetScene(nullptr);
    mUISystem.SetRenderer(nullptr);
}

void CApp::Render()
{
    mDebugRenderer.Render();
    mUISystem.DebugDraw();
}

} // namespace drash
