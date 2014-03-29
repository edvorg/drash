// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012-2014 Edward Knyshov, Yuriy Shatilin.

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
#ifndef CPLAYER_H
#define CPLAYER_H

#include "../scene/sceneobject.h"

namespace drash {

    class CSceneObject;

    class CPlayerParams final {
      public:
        CSceneObjectParams sceneObjectParams;
        float velocityLimit = 1;
    };

    class CPlayer final : public CObjectFactory<CPlayer>::CFactoryProduct {
      public:
        friend class CPlayersSystem;

        inline CSceneObject* GetSceneObject();

      protected:
      private:
        CSceneObject* sceneObject = nullptr;
        float velocityLimit = 1;
    };

    inline CSceneObject* CPlayer::GetSceneObject() { return sceneObject; }

} // namespace drash

#endif // CPLAYER_H
