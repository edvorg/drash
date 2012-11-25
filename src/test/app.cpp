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

namespace drash
{

bool CApp::Init()
{
    CSceneParams params;
    params.mGravity.Set( 0, -9.8 );

    if ( mScene.Init(params) == false )
    {
        return false;
    }

    mExplosionSystem.SetScene(&mScene);
    mPlayersSystem.SetScene(&mScene);
    mTemplateSystem.SetScene(&mScene);
    mDebugDrawSystem.SetScene(&mScene);

    CCamera *cam = GetDebugDrawSystem().CreateCam(CCameraParams(), true);

    if (cam == nullptr)
    {
        return false;
    }

    return true;
}

void CApp::Release()
{
    mPlayersSystem.SetScene(nullptr);
    mExplosionSystem.SetScene(nullptr);
    mTemplateSystem.SetScene(nullptr);
    mDebugDrawSystem.SetScene(nullptr);

    mScene.Release();
}

void CApp::PushEvent(const CAppEvent &_event)
{
    if (_event.GetType() != EventUnknown)
    {
        mEvents.push_back(_event);
    }
}

CAppEvent CApp::PopEvent()
{
    if (mEvents.size())
    {
        CAppEvent e(mEvents.back());
        mEvents.pop_back();
        return e;
    }
    else
    {
        return CAppEvent();
    }
}

} // namespace drash