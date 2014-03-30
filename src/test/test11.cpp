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

#include "test11.hpp"

#include "../greng/camera.h"
#include "../scene/sceneobject.h"
#include "../scene/figure.h"
#include <sstream>

namespace drash {

    namespace test {

        Test11::Test11(greng::GrengSystemsSet& greng) : App(greng) {
            camera = GetGrengSystems().GetCameraManager().CreateCamera({});
            camera->GetPos() = { -10, 10, 10 };
            camera->GetPos().SetTarget({ 10, -10, 10 }, 10,
                                       AnimatorBehavior::Bounce);
            camera->SetOrtho(true);
            
            camera->GetOrthoWidth() = 10;
            camera->GetOrthoHeight() =
                10.0 / GetGrengSystems().GetViewport().GetAspectRatio();
            
            camera->GetOrthoWidth().SetTarget(2, 5, AnimatorBehavior::Bounce);
            camera->GetOrthoHeight().SetTarget(
                2 / GetGrengSystems().GetViewport().GetAspectRatio(), 5,
                AnimatorBehavior::Bounce);

            uiCamera = GetGrengSystems().GetCameraManager().CreateCamera({});
            uiCamera->SetOrtho(true);
            uiCamera->GetOrthoWidth() = 100;
            uiCamera->GetOrthoHeight() =
                100.0 / GetGrengSystems().GetViewport().GetAspectRatio();
            uiCamera->GetPos() = { 0, 0, 10 };
            uiCamera->GetDepthOfView() = 50;

            GetDebugRenderer().SetCamera(camera);
            GetDebugRenderer().SetLight(&light);
            GetDebugRenderer().SetTexCoordsScale(0.3);

            FigureParams f;
            f.vertices.push_back({ -1, 0 });
            f.vertices.push_back({ 0, 1 });
            f.vertices.push_back({ 1, 0 });
            f.depth = 1;

            SceneObjectGeometry g;
            g.figures.push_back(f);

            SceneObjectParams p;
            p.dynamic = false;

            GetScene().CreateObject(g, p);

            GetEventSystem().SetProcessor("SPC", AppEventProcessor([this] {
                                                     camera->SetOrtho(
                                                         !camera->IsOrtho());
                                                 }));

            GetEventSystem().SetProcessor(
                "C-q", AppEventProcessor([this] { Quit(); }));
        }

        void Test11::Step(double _dt) {
            App::Step(_dt);
            camera->LookAt({ 0, 0, 0 });
            light.position = camera->GetPos();
            delta = _dt * 1000;
        }

        void Test11::Render() {
            App::Render();
            std::ostringstream str;
            str << "abcdefghijklmnopqrstuvwxyz_ " << delta;
            GetGrengSystems().GetRenderer().DrawString(
                *uiCamera, true,
                { -100 + 10, 100 / camera->GetAspectRatio() - 10 }, { 1, 1 },
                str.str());
        }

    } // namespace test

} // namespace drash
