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

#ifndef EXPLOSIONSYSTEM_H
#define EXPLOSIONSYSTEM_H

namespace drash
{

class CScene;

class CExplosionSystem final
{
public:    
    CExplosionSystem() = default;

    bool Init();
    void Step(double);
    void Release();

    inline void SetScene(CScene *_scene);
    inline CScene *GetScene();

protected:
private:
    CScene* mScene = nullptr;
};

inline void CExplosionSystem::SetScene(CScene *_scene)
{
    mScene = _scene;
}

inline CScene *CExplosionSystem::GetScene()
{
    return mScene;
}

}// namespace drash

#endif // EXPLOSIONSYSTEM_H