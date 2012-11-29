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
#ifndef CTESTAPP_H
#define CTESTAPP_H

#include "../sceneobjects.h"
#include "../cscene.h"
#include "../subsystem/explosionsystem.h"
#include "../subsystem/playerssystem.h"
#include "../subsystem/templatesystem.h"
#include "../subsystem/debugdrawsystem.h"
#include "appevent.h"

#include <list>

namespace drash
{

class CScene;

class CApp
{
public:
    virtual ~CApp() {}

    virtual bool Init();
    virtual void Release();
    inline virtual void Step(double _dt);
    inline virtual void Render();

    inline CScene &GetScene();
    inline const CScene &GetScene() const;

	inline CExplosionSystem &GetExplosionSystem();
    inline CPlayersSystem &GetPlayersSystem();
    inline CTemplateSystem &GetTemplateSystem();
    inline CDebugDrawSystem &GetDebugDrawSystem();

    void PushEvent(const CAppEvent &_event);
    CAppEvent PopEvent();

    /// used to make CApp childs about mouse moving event
    /// use this from your CApp back end (Qt, SDL, etc.)
    /// we assume that _pos is coordinates in screen space (-0.5, -0.5) (0.5, 0.5)
    inline void SetCursorPos(const CVec2 &_pos);

    /// used by CApp childs for detection, where mouse cursor is
    /// returns coordinates in screen space (-0.5, -0.5) (0.5, 0.5)
    inline const CVec2 &GetCursorPos() const;

protected:

private:
    CScene mScene;
    CExplosionSystem mExplosionSystem;
    CPlayersSystem mPlayersSystem;
    CTemplateSystem mTemplateSystem;
    CDebugDrawSystem mDebugDrawSystem;
    std::list<CAppEvent> mEvents;
    CVec2 mCursorPos = CVec2(0);
};

inline void CApp::Step(double _dt)
{
    mScene.Step(_dt);
}

inline void CApp::Render()
{
    mDebugDrawSystem.Draw();
}

inline CScene &CApp::GetScene()
{
    return mScene;
}

inline const CScene &CApp::GetScene() const
{
    return mScene;
}

inline CExplosionSystem &CApp::GetExplosionSystem()
{
    return mExplosionSystem;
}

inline CPlayersSystem &CApp::GetPlayersSystem()
{
    return mPlayersSystem;
}

inline CTemplateSystem &CApp::GetTemplateSystem()
{
    return mTemplateSystem;
}

inline CDebugDrawSystem &CApp::GetDebugDrawSystem()
{
    return mDebugDrawSystem;
}

inline void CApp::SetCursorPos(const CVec2 &_pos)
{
    mCursorPos = _pos;
}

inline const CVec2 &CApp::GetCursorPos() const
{
    return mCursorPos;
}

} // namespace drash

#endif // CTESTAPP_H
