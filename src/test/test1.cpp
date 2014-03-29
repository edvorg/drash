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

#include "test1.h"

#include "../scene/scene.h"
#include "../app/appeventprocessor.h"
#include "../greng/camera.h"
#include "../scene/figure.h"
#include "../scene/sceneobject.h"
#include <sstream>
#include <cstring>

namespace drash {

    namespace test {

        CTest1::CTest1() {
            GetGeometryManager().Load();

            greng::CCameraParams cp;
            cp.pos.Set(0, 0, 100);
            camera = GetGrengSystems().GetCameraManager().CreateCamera(cp);

            light1.position.Set(0, 0, 20);

            GetDebugRenderer().SetCamera(camera);
            GetDebugRenderer().SetLight(&light1);

            GetDebugRenderer().SetLight(&light1);

            SetProcessors();
        }

        void CTest1::Step(double _dt) {
            CApp::Step(_dt);

            if (currentFigure != nullptr && currentObject != nullptr) {
                center.Set(currentFigure->GetVertices()[0],
                           currentObject->GetPosZ() + currentFigure->GetZ());

                for (unsigned int i = 1; i < currentFigure->EnumVertices();
                     i++) {
                    center.Vec2() += currentFigure->GetVertices()[i];
                }

                center.Vec2() /= CVec2f(currentFigure->EnumVertices());

                x = center;
                y = center;
                z = center;
                x.x += 10;
                y.y += 10;
                z.z += 10;

                CPlane xz(PlaneXZ);
                xz.SetPoint(center);
                CPlane xy(PlaneXY);
                xy.SetPoint(center);

                CVec3f r1;
                camera->CastRay(GetCursorPos(), xz, r1);
                CVec3f r2;
                camera->CastRay(GetCursorPos(), xy, r2);

                CVec2f dstz = r1;
                dstz -= center.Vec2();
                CVec2f dstx(r1.z, r1.y);
                dstx -= CVec2f(center.z, center.y);
                CVec2f dsty(r2.z, r2.x);
                dsty -= CVec2f(center.z, center.x);

                axisOver = 0;
                axisDrawK.Set(1, 1, 1);

                if (dstz.Length() < 3) {
                    axisDrawK.z *= 0.5;
                    axisOver = 3;
                } else if (dstx.Length() < 3) {
                    axisDrawK.x *= 0.5;
                    axisOver = 1;
                } else if (dsty.Length() < 3) {
                    axisDrawK.y *= 0.5;
                    axisOver = 2;
                }
            }
        }

        float Area(const CVec2f& _p1, const CVec2f& _p2, const CVec2f& _p3) {
            return (_p2.x - _p1.x) * (_p3.y - _p1.y) -
                   (_p2.y - _p1.y) * (_p3.x - _p1.x);
        }

        float Intersect_1(float _x, float _y, float _a, float _b) {
            if (_x > _y)
                std::swap(_x, _y);
            if (_a > _b)
                std::swap(_a, _b);
            return math::Max(_x, _a) <= math::Min(_y, _b);
        }

        bool Intersect(const CVec2f& _p1, const CVec2f& _p2, const CVec2f& _p3,
                       const CVec2f& _p4) {
            return Intersect_1(_p1.x, _p2.x, _p3.x, _p4.x) &&
                   Intersect_1(_p1.y, _p2.y, _p3.y, _p4.y) &&
                   Area(_p1, _p2, _p3) * Area(_p1, _p2, _p4) <= 0 &&
                   Area(_p3, _p4, _p1) * Area(_p3, _p4, _p2) <= 0;
        }

        void CTest1::Render() {
            CApp::Render();

            if (vertices.size()) {
                for (int i = 0; i < (int)vertices.size() - 1; i++) {
                    GetGrengSystems().GetRenderer().DrawLine(
                        vertices[i], vertices[i + 1], 1, CColor4f(0, 1, 0, 1));
                }
                GetGrengSystems().GetRenderer().DrawLine(
                    vertices[vertices.size() - 1], GetCursorPos(), 1,
                    CColor4f(0, 1, 0, 1), false);
                GetGrengSystems().GetRenderer().DrawLine(
                    vertices[0], GetCursorPos(), 1, CColor4f(0, 1, 0, 1),
                    false);
            }

            if (currentFigure != nullptr && currentObject != nullptr) {
                GetGrengSystems().GetRenderer().DrawLine(
                    GetCamera(), center, x, 1,
                    CColor4f(1 * axisDrawK.x, 0, 0, 1), false);
                GetGrengSystems().GetRenderer().DrawLine(
                    GetCamera(), center, y, 1,
                    CColor4f(0, 1 * axisDrawK.y, 0, 1), false);
                GetGrengSystems().GetRenderer().DrawLine(
                    GetCamera(), center, z, 1,
                    CColor4f(0, 0, 1 * axisDrawK.z, 1), false);
            }

            if (splitMode == true && currentTemplate != nullptr &&
                currentObject != nullptr && currentFigure != nullptr) {
                if (splitDepth == false) {
                    GetGrengSystems().GetRenderer().DrawTriangle(
                        GetCamera(), splitPlanePoint1, splitPlanePoint2,
                        splitPlanePoint4, CColor4f(1, 0, 0.5, 0.5), true);
                    GetGrengSystems().GetRenderer().DrawTriangle(
                        GetCamera(), splitPlanePoint4, splitPlanePoint2,
                        splitPlanePoint3, CColor4f(1, 0, 0.5, 0.5), true);

                    if (splitIntersectionsCount == 2) {
                        auto draw_split = [&](CVec3f _split_intersection) {
                            CVec3f p1 = _split_intersection;
                            CVec3f p2 = _split_intersection;

                            p1.z = currentObject->GetPosZ() +
                                   currentFigure->GetZ() -
                                   currentFigure->GetDepth() * 0.5f;
                            p2.z = currentObject->GetPosZ() +
                                   currentFigure->GetZ() +
                                   currentFigure->GetDepth() * 0.5f;

                            GetGrengSystems().GetRenderer().DrawLine(
                                GetCamera(), p1, p2, 2, CColor4f(1, 1, 1),
                                false);
                        };

                        draw_split(splitIntersection1);
                        draw_split(splitIntersection2);
                    }
                } else {
                    CVec3f p1(splitFigureMin.Vec2(), splitFigureCenterZ);
                    CVec3f p2(splitFigureMin.x, splitFigureMax.y,
                              splitFigureCenterZ);
                    CVec3f p3(splitFigureMax.Vec2(), splitFigureCenterZ);
                    CVec3f p4(splitFigureMax.x, splitFigureMin.y,
                              splitFigureCenterZ);

                    GetGrengSystems().GetRenderer().DrawTriangle(
                        GetCamera(), p1, p2, p4, CColor4f(1, 0, 0.5, 0.5),
                        true);
                    GetGrengSystems().GetRenderer().DrawTriangle(
                        GetCamera(), p4, p2, p3, CColor4f(1, 0, 0.5, 0.5),
                        true);
                }
            }
        }

        CTest1::~CTest1() { GetGeometryManager().Store(); }

        void CTest1::SetProcessors() {
            GetEventSystem().SetMode(std::string("figure_movement"));

            GetEventSystem().SetProcessor(
                "C-s", CAppEventProcessor([this]() { BeginSplit(); }));

            GetEventSystem().SetProcessor(
                "WHUP", CAppEventProcessor([this]() { ExtrudeFigure(1); }));

            GetEventSystem().SetProcessor(
                "WHDN", CAppEventProcessor([this]() { ExtrudeFigure(-1); }));

            GetEventSystem().SetProcessor(
                "LB",
                CAppEventProcessor(
                    [this]() {
                        if (axisOver == 0) {
                            SelectFigure();
                        } else {
                            if (axisOver == 2) {
                                CPlane xy(PlaneXY);
                                xy.SetPoint(center);
                                camera->CastRay(GetCursorPos(), xy,
                                                figureMoveFirstClick);
                                axisMoving = axisOver;
                            } else {
                                CPlane xz(PlaneXZ);
                                xz.SetPoint(center);
                                camera->CastRay(GetCursorPos(), xz,
                                                figureMoveFirstClick);
                                axisMoving = axisOver;
                            }
                        }
                    },
                    [this]() {
                        if (axisMoving == 1) {
                            CPlane xz(PlaneXZ);
                            xz.SetPoint(center);
                            CVec3f new_pos;
                            camera->CastRay(GetCursorPos(), xz, new_pos);

                            CVec2f* v =
                                new CVec2f[currentFigure->EnumVertices()];
                            for (unsigned int i = 0;
                                 i < currentFigure->EnumVertices(); i++) {
                                v[i].x = currentFigure->GetVertices()[i].x +
                                         new_pos.x - figureMoveFirstClick.x;
                                v[i].y = currentFigure->GetVertices()[i].y;
                            }
                            currentFigure->SetVertices(
                                v, currentFigure->EnumVertices());
                            delete[] v;

                            figureMoveFirstClick = new_pos;

                            currentObject->DumpGeometry(currentTemplate);
                        } else if (axisMoving == 2) {
                            CPlane xy(PlaneXY);
                            xy.SetPoint(center);
                            CVec3f new_pos;
                            camera->CastRay(GetCursorPos(), xy, new_pos);

                            CVec2f* v =
                                new CVec2f[currentFigure->EnumVertices()];
                            for (unsigned int i = 0;
                                 i < currentFigure->EnumVertices(); i++) {
                                v[i].x = currentFigure->GetVertices()[i].x;
                                v[i].y = currentFigure->GetVertices()[i].y +
                                         new_pos.y - figureMoveFirstClick.y;
                            }
                            currentFigure->SetVertices(
                                v, currentFigure->EnumVertices());
                            delete[] v;

                            figureMoveFirstClick = new_pos;

                            currentObject->DumpGeometry(currentTemplate);
                        } else if (axisMoving == 3) {
                            CPlane xz(PlaneXZ);
                            xz.SetPoint(center);
                            CVec3f new_pos;
                            camera->CastRay(GetCursorPos(), xz, new_pos);

                            currentFigure->SetZ(currentFigure->GetZ() +
                                                new_pos.z -
                                                figureMoveFirstClick.z);

                            figureMoveFirstClick = new_pos;

                            currentObject->DumpGeometry(currentTemplate);
                        }
                    },
                    [this]() { axisMoving = 0; }));

            CamViewProcessors();

            ///

            GetEventSystem().SetMode(std::string("split_mode"));

            GetEventSystem().SetProcessor(
                "C-s", CAppEventProcessor([this]() { EndSplit(); }));

            GetEventSystem().SetProcessor("C-v", CAppEventProcessor([this]() {
                                                     splitDepth = !splitDepth;
                                                 }));

            GetEventSystem().SetProcessor(
                "C-r", CAppEventProcessor([this]() {
                           CMatrix4f m;
                           MatrixRotationZ(m, M_PI / 12);
                           CVec4f old_normal(splitPlane.GetNormal(), 1);
                           CVec4f new_normal;
                           MatrixMultiply(m, old_normal, new_normal);
                           splitPlane.SetNormal(new_normal);
                           ComputeIntersections();
                       }));

            CamViewProcessors();

            ///

            GetEventSystem().SetMode(std::string("editor_mode"));

            GetEventSystem().SetProcessor(
                "C-c", CAppEventProcessor([this]() { CreateGeometry(); }));

            GetEventSystem().SetProcessor(
                "C-d", CAppEventProcessor([this]() { DetachCurrentObject(); }));

            GetEventSystem().SetProcessor("C-LB", CAppEventProcessor([this]() {
                                                      vertices.push_back(
                                                          GetCursorPos());
                                                  }));

            GetEventSystem().SetProcessor(
                "LB", CAppEventProcessor([this]() { SelectFigure(); }));

            GetEventSystem().SetProcessor(
                "RB", CAppEventProcessor([this]() { CompleteFigure(); }));

            CamViewProcessors();

            GetEventSystem().SetProcessor(
                "C-q", CAppEventProcessor([this]() { this->Quit(); }));

            GetEventSystem().SetProcessor(
                "e",
                CAppEventProcessor([this]() {
                    if (currentTemplate == nullptr) {
                        currentTemplate = GetGeometryManager()
                                              .GetGeometries()
                                              .begin()
                                              ->second;

                        if (currentObject != nullptr) {
                            GetScene().DestroyObject(currentObject);
                        }

                        CSceneObjectParams p;
                        p.dynamic = false;
                        currentObject =
                            GetScene().CreateObject(*currentTemplate, p);
                        currentObject->SetActive(false);
                    } else {
                        auto& t = GetGeometryManager().GetGeometries();
                        for (auto i = t.begin(); i != t.end(); i++) {
                            if (currentTemplate == i->second) {
                                i++;

                                if (i == t.end()) {
                                    break;
                                }

                                currentTemplate = i->second;

                                if (currentObject != nullptr) {
                                    GetScene().DestroyObject(currentObject);
                                }

                                CSceneObjectParams p;
                                p.dynamic = false;
                                currentObject = GetScene().CreateObject(
                                    *currentTemplate, p);
                                currentObject->SetActive(false);

                                break;
                            }
                        }
                    }
                }));
        }

        void CTest1::BeginSplit() {
            if (currentTemplate != nullptr && currentObject != nullptr &&
                currentFigure != nullptr) {
                GetEventSystem().SetMode(std::string("split_mode"));
                splitMode = true;

                splitFigureMin.Set(currentFigure->GetVertices()[0].x,
                                   currentFigure->GetVertices()[0].y,
                                   currentObject->GetPosZ() +
                                       currentFigure->GetZ());
                splitFigureMax.Set(currentFigure->GetVertices()[0].x,
                                   currentFigure->GetVertices()[0].y,
                                   currentObject->GetPosZ() +
                                       currentFigure->GetZ());

                for (unsigned int i = 0; i < currentFigure->EnumVertices();
                     i++) {
                    splitFigureMin.x = math::Min<float>(
                        splitFigureMin.x, currentFigure->GetVertices()[i].x);
                    splitFigureMax.x = math::Max<float>(
                        splitFigureMax.x, currentFigure->GetVertices()[i].x);
                    splitFigureMin.y = math::Min<float>(
                        splitFigureMin.y, currentFigure->GetVertices()[i].y);
                    splitFigureMax.y = math::Max<float>(
                        splitFigureMax.y, currentFigure->GetVertices()[i].y);
                    splitFigureMin.z = math::Min<float>(
                        splitFigureMin.z,
                        currentObject->GetPosZ() + currentFigure->GetZ());
                    splitFigureMax.z = math::Max<float>(
                        splitFigureMax.z,
                        currentObject->GetPosZ() + currentFigure->GetZ());
                }

                splitFigureMin -= 10;
                splitFigureMax += 10;

                splitFigureMin.z -= currentFigure->GetDepth() * 0.5;
                splitFigureMax.z += currentFigure->GetDepth() * 0.5;

                splitPlane.SetNormal(CVec3f(0, 1, 0));
                splitPlane.SetPoint(
                    CVec3f(0.5f * (splitFigureMin.x + splitFigureMax.x),
                           0.5f * (splitFigureMin.y + splitFigureMax.y),
                           0.5f * (splitFigureMin.z + splitFigureMax.z)));

                splitFigureCenterZ =
                    0.5f * (splitFigureMin.z + splitFigureMax.z);

                ComputeIntersections();
            }
        }

        void CTest1::DetectNewSplitPoint(const CVec2f& _p1, const CVec2f& _p2,
                                         unsigned int _index, const CRay& _r) {
            float centerz = currentObject->GetPosZ() + currentFigure->GetZ();

            CPlane p;
            p.Set(CVec3f(_p1, centerz), CVec3f(_p2, centerz),
                  CVec3f(_p2, centerz - 1));

            CVec3f p1;
            CVec3f p2;
            p.CastRay(_r, p1);
            p2 = p1;

            if (splitIntersectionsCount == 0) {
                splitIntersection1 = p1;
                splitIntersection1Index = _index;
            } else if (splitIntersectionsCount == 1) {
                splitIntersection2 = p1;
                splitIntersection2Index = _index;
            }

            if (Intersect(_p1, _p2, splitPlanePoint4, splitPlanePoint1)) {
                splitIntersectionsCount++;
            }
        }

        void CTest1::ComputeIntersections() {
            if (currentTemplate != nullptr && currentObject != nullptr &&
                currentFigure != nullptr) {
                CRay r;

                splitPlanePoint1.Set(splitFigureMin.x, 0, splitFigureMax.z);
                splitPlanePoint2.Set(splitFigureMin.x, 0, splitFigureMin.z);
                splitPlanePoint3.Set(splitFigureMax.x, 0, splitFigureMin.z);
                splitPlanePoint4.Set(splitFigureMax.x, 0, splitFigureMax.z);

                r.SetDirection(CVec3f(0, -1, 0));

                r.SetPoint(splitPlanePoint1);
                splitPlane.CastRay(r, splitPlanePoint1);
                r.SetPoint(splitPlanePoint2);
                splitPlane.CastRay(r, splitPlanePoint2);
                r.SetPoint(splitPlanePoint3);
                splitPlane.CastRay(r, splitPlanePoint3);
                r.SetPoint(splitPlanePoint4);
                splitPlane.CastRay(r, splitPlanePoint4);

                CVec3f dir = splitPlanePoint1;
                dir -= splitPlanePoint4;

                float centerz =
                    currentObject->GetPosZ() + currentFigure->GetZ();

                r.SetPoint(CVec3f(splitPlanePoint4.Vec2(), centerz));
                r.SetDirection(dir);

                splitIntersectionsCount = 0;

                if (currentFigure->EnumVertices() != 0) {
                    for (unsigned int i = 1; i < currentFigure->EnumVertices();
                         i++) {
                        DetectNewSplitPoint(currentFigure->GetVertices()[i - 1],
                                            currentFigure->GetVertices()[i],
                                            i - 1, r);
                    }
                    DetectNewSplitPoint(currentFigure->GetVertices()
                                            [currentFigure->EnumVertices() - 1],
                                        currentFigure->GetVertices()[0],
                                        currentFigure->EnumVertices() - 1, r);
                }
            }
        }

        void CTest1::EndSplit() {
            GetEventSystem().SetMode(std::string("figure_movement"));
            splitMode = false;

            if (currentObject != nullptr) {
                if (currentFigure != nullptr && currentTemplate != nullptr) {
                    if (splitDepth == false) {
                        if (splitIntersectionsCount == 2) {
                            unsigned int fsize = splitIntersection1Index + 2 +
                                                 currentFigure->EnumVertices() -
                                                 splitIntersection2Index;

                            CFigureParams fp;
                            fp.vertices.resize(fsize);
                            fp.depth = currentFigure->GetDepth();
                            fp.z = currentFigure->GetZ();

                            unsigned int i = 0;

                            for (; i <= splitIntersection1Index; i++) {
                                fp.vertices[i] =
                                    currentFigure->GetVertices()[i];
                            }
                            fp.vertices[i++] = splitIntersection1;
                            fp.vertices[i++] = splitIntersection2;
                            for (i = splitIntersection2Index + 1;
                                 i < currentFigure->EnumVertices(); i++) {
                                fp.vertices[i] =
                                    currentFigure->GetVertices()[i];
                            }

                            currentObject->CreateFigure(fp);

                            fsize = splitIntersection2Index -
                                    splitIntersection1Index + 2;

                            fp.vertices.clear();
                            fp.vertices.resize(fsize);

                            i = 0;
                            fp.vertices[i++] = splitIntersection1;
                            for (unsigned int j = splitIntersection1Index + 1;
                                 j <= splitIntersection2Index; j++) {
                                fp.vertices[i++] =
                                    currentFigure->GetVertices()[j];
                            }
                            fp.vertices[i] = splitIntersection2;

                            currentObject->CreateFigure(fp);

                            currentObject->DestroyFigure(currentFigure);
                            currentFigure = nullptr;
                            currentObject->DumpGeometry(currentTemplate);

                            GetEventSystem().SetMode(
                                std::string("editor_mode"));
                        }
                    } else {
                        CFigureParams fp;
                        fp.vertices.resize(currentFigure->EnumVertices());
                        memcpy(&fp.vertices[0], currentFigure->GetVertices(),
                               sizeof(CVec2f) * currentFigure->EnumVertices());

                        float min = currentObject->GetPosZ() +
                                    currentFigure->GetZ() +
                                    0.5f * currentFigure->GetDepth();
                        float max = currentObject->GetPosZ() +
                                    currentFigure->GetZ() -
                                    0.5f * currentFigure->GetDepth();

                        fp.z = 0.5f * (splitFigureCenterZ + min);
                        fp.depth = math::Abs(splitFigureCenterZ - min);
                        currentObject->CreateFigure(fp);

                        fp.z = 0.5f * (splitFigureCenterZ + max);
                        fp.depth = math::Abs(max - splitFigureCenterZ);
                        currentObject->CreateFigure(fp);

                        currentObject->DestroyFigure(currentFigure);
                        currentFigure = nullptr;
                        currentObject->DumpGeometry(currentTemplate);

                        GetEventSystem().SetMode(std::string("editor_mode"));
                    }
                }
            }
        }

        void CTest1::ExtrudeFigure(float _delta_depth) {
            if (currentFigure != nullptr) {
                currentFigure->SetDepth(math::Max<float>(
                    0, currentFigure->GetDepth() + _delta_depth));

                currentObject->DumpGeometry(currentTemplate);
            }
        }

        void CTest1::CamViewProcessors() {
            GetEventSystem().SetProcessor(
                "MB", CAppEventProcessor([this]() {
                                             camRotFirstClick = GetCursorPos();
                                         },
                                         [this]() {
                          CVec2f new_pos = GetCursorPos();

                          CVec2f rot = camera->GetRotation().Get();
                          rot.y -= new_pos.x - camRotFirstClick.x;
                          rot.x += new_pos.y - camRotFirstClick.y;

                          camera->GetRotation().Set(rot);

                          camRotFirstClick = new_pos;
                      }));

            GetEventSystem().SetProcessor(
                "w", CAppEventProcessor([this]() {}, [this]() {
                         camera->Forward(50 * GetCurrentTimeDelta());
                     }));

            GetEventSystem().SetProcessor(
                "a", CAppEventProcessor([this]() {}, [this]() {
                         camera->Strafe(50 * GetCurrentTimeDelta());
                     }));

            GetEventSystem().SetProcessor(
                "s", CAppEventProcessor([this]() {}, [this]() {
                         camera->Forward(-50 * GetCurrentTimeDelta());
                     }));

            GetEventSystem().SetProcessor(
                "d", CAppEventProcessor([this]() {}, [this]() {
                         camera->Strafe(-50 * GetCurrentTimeDelta());
                     }));

            GetEventSystem().SetProcessor(
                "q",
                CAppEventProcessor([this]() {
                    if (currentTemplate == nullptr) {
                        currentTemplate =
                            (--GetGeometryManager().GetGeometries().end())
                                ->second;

                        if (currentObject != nullptr) {
                            GetScene().DestroyObject(currentObject);
                        }

                        CSceneObjectParams p;
                        p.dynamic = false;
                        currentObject =
                            GetScene().CreateObject(*currentTemplate, p);
                        currentObject->SetActive(false);
                    } else {
                        auto& t = GetGeometryManager().GetGeometries();
                        for (auto i = t.begin(); i != t.end(); i++) {
                            if (currentTemplate == i->second) {
                                if (i == t.begin()) {
                                    break;
                                }

                                i--;

                                currentTemplate = i->second;

                                if (currentObject != nullptr) {
                                    GetScene().DestroyObject(currentObject);
                                }

                                CSceneObjectParams p;
                                p.dynamic = false;
                                currentObject = GetScene().CreateObject(
                                    *currentTemplate, p);
                                currentObject->SetActive(false);

                                break;
                            }
                        }
                    }
                }));
        }

        void CTest1::CompleteFigure() {
            if (currentTemplate != nullptr && vertices.size() >= 3) {
                currentTemplate->figures.resize(
                    currentTemplate->figures.size() + 1);

                currentTemplate->figures.back().vertices = vertices;

                for (auto i = currentTemplate->figures.back().vertices.begin();
                     i != currentTemplate->figures.back().vertices.end(); i++) {
                    CPlane plane;
                    plane.SetNormal(CVec3f(0, 0, 1));
                    plane.SetPoint(CVec3f(0, 0, 0));

                    CVec3f pos;

                    camera->CastRay(*i, plane, pos);

                    *i = pos;
                }

                if (currentObject != nullptr) {
                    GetScene().DestroyObject(currentObject);
                    this->currentObject = nullptr;
                }

                CSceneObjectParams p;
                p.dynamic = false;
                currentObject = GetScene().CreateObject(*currentTemplate, p);
                currentObject->SetActive(false);

                vertices.clear();
            }
        }

        void CTest1::CreateGeometry() {
            static const unsigned max_try_count = 10;
            unsigned int counter = 0;

            do {
                std::ostringstream is;
                is << "new_geometry_" << (templateCounter++);
                this->currentTemplate =
                    GetGeometryManager().CreateGeometry(is.str().c_str());
            } while (this->currentTemplate == nullptr &&
                     counter < max_try_count);

            if (currentObject != nullptr) {
                GetScene().DestroyObject(currentObject);
                this->currentObject = nullptr;
            }
        }

        void CTest1::DetachCurrentObject() {
            if (currentObject != nullptr) {
                currentObject->SetDynamic(true);
                currentObject->SetActive(true);

                if (currentTemplate != nullptr) {
                    CSceneObjectParams p;
                    p.dynamic = false;
                    currentObject =
                        GetScene().CreateObject(*currentTemplate, p);
                    currentObject->SetActive(false);
                }
            }
        }

        void CTest1::SelectFigure() {
            currentFigure =
                GetDebugRenderer().FindFigure(*camera, GetCursorPos());

            if (currentFigure != nullptr) {
                GetEventSystem().SetMode("figure_movement");
            } else {
                GetEventSystem().SetMode("editor_mode");
            }
        }

    } // namespace test

} // namespace drash
