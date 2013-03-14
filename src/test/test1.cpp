// DRASH_LICENSE_BEGIN
/*

drash GPL Source Code
Copyright (C) 2012-2013 Edward Knyshov, Yuriy Shatilin.

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

namespace drash
{

namespace test
{

bool CTest1::Init()
{
    if ( CApp::Init() == false )
    {
        return false;
    }

    if (GetGeometryManager().Load() == false)
    {
        return false;
    }

    greng::CCameraParams cp;
    cp.mPos.Set(0, 0, 100);
    mCamera = GetGrengSystems().GetCameraManager().CreateCamera(cp);

    if (mCamera == nullptr)
    {
        return false;
    }

    mLight1.mPosition.Set(0, 0, 20);

    GetDebugRenderer().SetCamera(mCamera);
    GetDebugRenderer().SetLight(&mLight1);

    GetDebugRenderer().SetLight(&mLight1);

    SetProcessors();

    return true;
}

void CTest1::Step(double _dt)
{
    CApp::Step(_dt);


    if (mCurrentFigure != nullptr && mCurrentObject != nullptr)
    {
        mCenter.Set(mCurrentFigure->GetVertices()[0], mCurrentObject->GetPosZ() + mCurrentFigure->GetZ());

        for (unsigned int i = 1; i < mCurrentFigure->EnumVertices(); i++)
        {
            mCenter.Vec2() += mCurrentFigure->GetVertices()[i];
        }

        mCenter.Vec2() /= CVec2f(mCurrentFigure->EnumVertices());

        mX = mCenter;
        mY = mCenter;
        mZ = mCenter;
        mX.mX += 10;
        mY.mY += 10;
        mZ.mZ += 10;

        CPlane xz(PlaneXZ);
        xz.SetPoint(mCenter);
        CPlane xy(PlaneXY);
        xy.SetPoint(mCenter);

        CVec3f r1;
        mCamera->CastRay(GetCursorPos(), xz, r1);
        CVec3f r2;
        mCamera->CastRay(GetCursorPos(), xy, r2);

        CVec2f dstz = r1;
        dstz -= mCenter.Vec2();
        CVec2f dstx(r1.mZ, r1.mY);
        dstx -= CVec2f(mCenter.mZ, mCenter.mY);
        CVec2f dsty(r2.mZ, r2.mX);
        dsty -= CVec2f(mCenter.mZ, mCenter.mX);

        mAxisOver = 0;
        mAxisDrawK.Set(1, 1, 1);

        if (dstz.Length() < 3)
        {
            mAxisDrawK.mZ *= 0.5;
            mAxisOver = 3;
        }
        else if (dstx.Length() < 3)
        {
            mAxisDrawK.mX *= 0.5;
            mAxisOver = 1;
        }
        else if (dsty.Length() < 3)
        {
            mAxisDrawK.mY *= 0.5;
            mAxisOver = 2;
        }
    }
}

float Area(const CVec2f &_p1,
           const CVec2f &_p2,
           const CVec2f &_p3)
{
    return (_p2.mX - _p1.mX) * (_p3.mY - _p1.mY) -
           (_p2.mY - _p1.mY) * (_p3.mX - _p1.mX);
}

float Intersect_1(float _x,
                  float _y,
                  float _a,
                  float _b)
{
    if (_x > _y) std::swap(_x, _y);
    if (_a > _b) std::swap(_a, _b);
    return math::Max(_x, _a) <= math::Min(_y, _b);
}

bool Intersect(const CVec2f &_p1,
               const CVec2f &_p2,
               const CVec2f &_p3,
               const CVec2f &_p4)
{
    return Intersect_1(_p1.mX, _p2.mX, _p3.mX, _p4.mX) &&
           Intersect_1(_p1.mY, _p2.mY, _p3.mY, _p4.mY) &&
           Area(_p1, _p2, _p3) * Area(_p1, _p2, _p4) <= 0 &&
           Area(_p3, _p4, _p1) * Area(_p3, _p4, _p2) <= 0;
}

void CTest1::Render()
{
    CApp::Render();

    if (mVertices.size())
    {
        for (int i = 0; i < (int)mVertices.size() - 1; i++)
        {
            GetGrengSystems().GetRenderer().DrawLine(mVertices[i], mVertices[i+1], 1, CColor4f(0, 1, 0, 1));
        }
        GetGrengSystems().GetRenderer().DrawLine(mVertices[mVertices.size()-1],
                               GetCursorPos(),
                               1,
                               CColor4f(0, 1, 0, 1),
                               false);
        GetGrengSystems().GetRenderer().DrawLine(mVertices[0], GetCursorPos(), 1, CColor4f(0, 1, 0, 1), false);
    }

    if (mCurrentFigure != nullptr && mCurrentObject != nullptr)
    {
        GetGrengSystems().GetRenderer().DrawLine(GetCamera(), mCenter, mX, 1, CColor4f(1 * mAxisDrawK.mX, 0, 0, 1), false);
        GetGrengSystems().GetRenderer().DrawLine(GetCamera(), mCenter, mY, 1, CColor4f(0, 1 * mAxisDrawK.mY, 0, 1), false);
        GetGrengSystems().GetRenderer().DrawLine(GetCamera(), mCenter, mZ, 1, CColor4f(0, 0, 1 * mAxisDrawK.mZ, 1), false);
    }

    if (mSplitMode == true &&
        mCurrentTemplate != nullptr &&
        mCurrentObject != nullptr &&
        mCurrentFigure != nullptr)
    {
        if (mSplitDepth == false)
        {
            GetGrengSystems().GetRenderer().DrawTriangle(GetCamera(),
                                       mSplitPlanePoint1,
                                       mSplitPlanePoint2,
                                       mSplitPlanePoint4,
                                       CColor4f(1, 0, 0.5, 0.5),
                                       true);
            GetGrengSystems().GetRenderer().DrawTriangle(GetCamera(),
                                       mSplitPlanePoint4,
                                       mSplitPlanePoint2,
                                       mSplitPlanePoint3,
                                       CColor4f(1, 0, 0.5, 0.5),
                                       true);

            if (mSplitIntersectionsCount == 2)
            {
                auto draw_split = [&] (CVec3f _split_intersection)
                {
                    CVec3f p1 = _split_intersection;
                    CVec3f p2 = _split_intersection;

                    p1.mZ = mCurrentObject->GetPosZ() + mCurrentFigure->GetZ() - mCurrentFigure->GetDepth() * 0.5f;
                    p2.mZ = mCurrentObject->GetPosZ() + mCurrentFigure->GetZ() + mCurrentFigure->GetDepth() * 0.5f;

                    GetGrengSystems().GetRenderer().DrawLine(GetCamera(), p1, p2, 2, CColor4f(1, 1, 1), false);
                };

                draw_split(mSplitIntersection1);
                draw_split(mSplitIntersection2);
            }
        }
        else
        {
            CVec3f p1(mSplitFigureMin.Vec2(), mSplitFigureCenterZ);
            CVec3f p2(mSplitFigureMin.mX, mSplitFigureMax.mY, mSplitFigureCenterZ);
            CVec3f p3(mSplitFigureMax.Vec2(), mSplitFigureCenterZ);
            CVec3f p4(mSplitFigureMax.mX, mSplitFigureMin.mY, mSplitFigureCenterZ);

            GetGrengSystems().GetRenderer().DrawTriangle(GetCamera(), p1, p2, p4, CColor4f(1, 0, 0.5, 0.5), true);
            GetGrengSystems().GetRenderer().DrawTriangle(GetCamera(), p4, p2, p3, CColor4f(1, 0, 0.5, 0.5), true);
        }
    }
}

void CTest1::Release()
{
    GetGeometryManager().Store();

    CApp::Release();
}

void CTest1::SetProcessors()
{
    GetEventSystem().SetMode(std::string("figure_movement"));

    GetEventSystem().SetProcessor("C-s", CAppEventProcessor(
    [this] ()
    {
        BeginSplit();
    }));

    GetEventSystem().SetProcessor("WHUP", CAppEventProcessor(
    [this] ()
    {
        ExtrudeFigure(1);
    }));

    GetEventSystem().SetProcessor("WHDN", CAppEventProcessor(
    [this] ()
    {
        ExtrudeFigure(-1);
    }));

    GetEventSystem().SetProcessor("LB", CAppEventProcessor(
    [this] ()
    {
        if (mAxisOver == 0)
        {
            SelectFigure();
        }
        else
        {
            if (mAxisOver == 2)
            {
                CPlane xy(PlaneXY);
                xy.SetPoint(mCenter);
                mCamera->CastRay(GetCursorPos(), xy, mFigureMoveFirstClick);
                mAxisMoving = mAxisOver;
            }
            else
            {
                CPlane xz(PlaneXZ);
                xz.SetPoint(mCenter);
                mCamera->CastRay(GetCursorPos(), xz, mFigureMoveFirstClick);
                mAxisMoving = mAxisOver;
            }
        }
    },
    [this] ()
    {
        if (mAxisMoving == 1)
        {
            CPlane xz(PlaneXZ);
            xz.SetPoint(mCenter);
            CVec3f new_pos;
            mCamera->CastRay(GetCursorPos(), xz, new_pos);

            CVec2f *v = new CVec2f[mCurrentFigure->EnumVertices()];
            for (unsigned int i = 0; i < mCurrentFigure->EnumVertices(); i++)
            {
                v[i].mX = mCurrentFigure->GetVertices()[i].mX + new_pos.mX - mFigureMoveFirstClick.mX;
                v[i].mY = mCurrentFigure->GetVertices()[i].mY;
            }
            mCurrentFigure->SetVertices(v, mCurrentFigure->EnumVertices());
            delete [] v;

            mFigureMoveFirstClick = new_pos;

            mCurrentObject->DumpGeometry(mCurrentTemplate);
        }
        else if (mAxisMoving == 2)
        {
            CPlane xy(PlaneXY);
            xy.SetPoint(mCenter);
            CVec3f new_pos;
            mCamera->CastRay(GetCursorPos(), xy, new_pos);

            CVec2f *v = new CVec2f[mCurrentFigure->EnumVertices()];
            for (unsigned int i = 0; i < mCurrentFigure->EnumVertices(); i++)
            {
                v[i].mX = mCurrentFigure->GetVertices()[i].mX;
                v[i].mY = mCurrentFigure->GetVertices()[i].mY + new_pos.mY - mFigureMoveFirstClick.mY;
            }
            mCurrentFigure->SetVertices(v, mCurrentFigure->EnumVertices());
            delete [] v;

            mFigureMoveFirstClick = new_pos;

            mCurrentObject->DumpGeometry(mCurrentTemplate);
        }
        else if (mAxisMoving == 3)
        {
            CPlane xz(PlaneXZ);
            xz.SetPoint(mCenter);
            CVec3f new_pos;
            mCamera->CastRay(GetCursorPos(), xz, new_pos);

            mCurrentFigure->SetZ(mCurrentFigure->GetZ() + new_pos.mZ - mFigureMoveFirstClick.mZ);

            mFigureMoveFirstClick = new_pos;

            mCurrentObject->DumpGeometry(mCurrentTemplate);
        }
    },
    [this] ()
    {
        mAxisMoving = 0;
    }));

    CamViewProcessors();

    ///

    GetEventSystem().SetMode(std::string("split_mode"));

    GetEventSystem().SetProcessor("C-s", CAppEventProcessor(
    [this] ()
    {
        EndSplit();
    }));

    GetEventSystem().SetProcessor("C-v", CAppEventProcessor(
    [this] ()
    {
        mSplitDepth = !mSplitDepth;
    }));

    GetEventSystem().SetProcessor("C-r", CAppEventProcessor(
    [this] ()
    {
        CMatrix4f m;
        MatrixRotationZ(m, M_PI / 12);
        CVec4f old_normal(mSplitPlane.GetNormal(), 1);
        CVec4f new_normal;
        MatrixMultiply(m, old_normal, new_normal);
        mSplitPlane.SetNormal(new_normal);
        ComputeIntersections();
    }));

    CamViewProcessors();

    ///

    GetEventSystem().SetMode(std::string("editor_mode"));

    GetEventSystem().SetProcessor("C-c", CAppEventProcessor(
    [this] ()
    {
        CreateGeometry();
    }));

    GetEventSystem().SetProcessor("C-d", CAppEventProcessor(
    [this] ()
    {
        DetachCurrentObject();
    }));

    GetEventSystem().SetProcessor("C-LB", CAppEventProcessor(
    [this] ()
    {
        mVertices.push_back(GetCursorPos());
    }));

    GetEventSystem().SetProcessor("LB", CAppEventProcessor(
    [this] ()
    {
        SelectFigure();
    }));

    GetEventSystem().SetProcessor("RB", CAppEventProcessor(
    [this] ()
    {
        CompleteFigure();
    }));

    CamViewProcessors();

    GetEventSystem().SetProcessor("C-q", CAppEventProcessor(
    [this] ()
    {
        this->Quit();
    }));

    GetEventSystem().SetProcessor("e", CAppEventProcessor(
    [this] ()
    {
        if (mCurrentTemplate == nullptr)
        {
            mCurrentTemplate = GetGeometryManager().GetGeometries().begin()->second;

            if (mCurrentObject != nullptr)
            {
                GetScene().DestroyObject(mCurrentObject);
            }

            CSceneObjectParams p;
            p.mDynamic = false;
            mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
            mCurrentObject->SetActive(false);
        }
        else
        {
            auto &t = GetGeometryManager().GetGeometries();
            for (auto i = t.begin(); i != t.end(); i++)
            {
                if (mCurrentTemplate == i->second)
                {
                    i++;

                    if (i == t.end())
                    {
                        break;
                    }

                    mCurrentTemplate = i->second;

                    if (mCurrentObject != nullptr)
                    {
                        GetScene().DestroyObject(mCurrentObject);
                    }

                    CSceneObjectParams p;
                    p.mDynamic = false;
                    mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
                    mCurrentObject->SetActive(false);

                    break;
                }
            }
        }
    }));
}

void CTest1::BeginSplit()
{
    if (mCurrentTemplate != nullptr && mCurrentObject != nullptr && mCurrentFigure != nullptr)
    {
        GetEventSystem().SetMode(std::string("split_mode"));
        mSplitMode = true;

        mSplitFigureMin.Set(mCurrentFigure->GetVertices()[0].mX,
                            mCurrentFigure->GetVertices()[0].mY,
                            mCurrentObject->GetPosZ() + mCurrentFigure->GetZ());
        mSplitFigureMax.Set(mCurrentFigure->GetVertices()[0].mX,
                            mCurrentFigure->GetVertices()[0].mY,
                            mCurrentObject->GetPosZ() + mCurrentFigure->GetZ());

        for (unsigned int i = 0; i < mCurrentFigure->EnumVertices(); i++)
        {
            mSplitFigureMin.mX = math::Min<float>(mSplitFigureMin.mX, mCurrentFigure->GetVertices()[i].mX);
            mSplitFigureMax.mX = math::Max<float>(mSplitFigureMax.mX, mCurrentFigure->GetVertices()[i].mX);
            mSplitFigureMin.mY = math::Min<float>(mSplitFigureMin.mY, mCurrentFigure->GetVertices()[i].mY);
            mSplitFigureMax.mY = math::Max<float>(mSplitFigureMax.mY, mCurrentFigure->GetVertices()[i].mY);
            mSplitFigureMin.mZ = math::Min<float>(mSplitFigureMin.mZ, mCurrentObject->GetPosZ() + mCurrentFigure->GetZ());
            mSplitFigureMax.mZ = math::Max<float>(mSplitFigureMax.mZ, mCurrentObject->GetPosZ() + mCurrentFigure->GetZ());
        }

        mSplitFigureMin -= 10;
        mSplitFigureMax += 10;

        mSplitFigureMin.mZ -= mCurrentFigure->GetDepth() * 0.5;
        mSplitFigureMax.mZ += mCurrentFigure->GetDepth() * 0.5;

        mSplitPlane.SetNormal(CVec3f(0, 1, 0));
        mSplitPlane.SetPoint(CVec3f(0.5f * (mSplitFigureMin.mX + mSplitFigureMax.mX),
                                    0.5f * (mSplitFigureMin.mY + mSplitFigureMax.mY),
                                    0.5f * (mSplitFigureMin.mZ + mSplitFigureMax.mZ)));


        mSplitFigureCenterZ = 0.5f * (mSplitFigureMin.mZ + mSplitFigureMax.mZ);

        ComputeIntersections();
    }
}

void CTest1::DetectNewSplitPoint(const CVec2f &_p1, const CVec2f &_p2, unsigned int _index, const CRay &_r)
{
    float centerz = mCurrentObject->GetPosZ() + mCurrentFigure->GetZ();

    CPlane p;
    p.Set(CVec3f(_p1, centerz),
          CVec3f(_p2, centerz),
          CVec3f(_p2, centerz - 1));

    CVec3f p1;
    CVec3f p2;
    p.CastRay(_r, p1);
    p2 = p1;

    if (mSplitIntersectionsCount == 0)
    {
        mSplitIntersection1 = p1;
        mSplitIntersection1Index = _index;
    }
    else if (mSplitIntersectionsCount == 1)
    {
        mSplitIntersection2 = p1;
        mSplitIntersection2Index = _index;
    }

    if (Intersect(_p1, _p2, mSplitPlanePoint4, mSplitPlanePoint1))
    {
        mSplitIntersectionsCount++;
    }
}

void CTest1::ComputeIntersections()
{
    if (mCurrentTemplate != nullptr && mCurrentObject != nullptr && mCurrentFigure != nullptr)
    {
        CRay r;

        mSplitPlanePoint1.Set(mSplitFigureMin.mX, 0, mSplitFigureMax.mZ);
        mSplitPlanePoint2.Set(mSplitFigureMin.mX, 0, mSplitFigureMin.mZ);
        mSplitPlanePoint3.Set(mSplitFigureMax.mX, 0, mSplitFigureMin.mZ);
        mSplitPlanePoint4.Set(mSplitFigureMax.mX, 0, mSplitFigureMax.mZ);

        r.SetDirection(CVec3f(0, -1, 0));

        r.SetPoint(mSplitPlanePoint1);
        mSplitPlane.CastRay(r, mSplitPlanePoint1);
        r.SetPoint(mSplitPlanePoint2);
        mSplitPlane.CastRay(r, mSplitPlanePoint2);
        r.SetPoint(mSplitPlanePoint3);
        mSplitPlane.CastRay(r, mSplitPlanePoint3);
        r.SetPoint(mSplitPlanePoint4);
        mSplitPlane.CastRay(r, mSplitPlanePoint4);

        CVec3f dir = mSplitPlanePoint1;
        dir -= mSplitPlanePoint4;

        float centerz = mCurrentObject->GetPosZ() + mCurrentFigure->GetZ();

        r.SetPoint(CVec3f(mSplitPlanePoint4.Vec2(), centerz));
        r.SetDirection(dir);

        mSplitIntersectionsCount = 0;

        if (mCurrentFigure->EnumVertices() != 0)
        {
            for (unsigned int i = 1; i < mCurrentFigure->EnumVertices(); i++)
            {
                DetectNewSplitPoint(mCurrentFigure->GetVertices()[i-1], mCurrentFigure->GetVertices()[i], i-1, r);
            }
            DetectNewSplitPoint(mCurrentFigure->GetVertices()[mCurrentFigure->EnumVertices()-1],
                                mCurrentFigure->GetVertices()[0],
                                mCurrentFigure->EnumVertices()-1,
                                r);
        }
    }
}

void CTest1::EndSplit()
{
    GetEventSystem().SetMode(std::string("figure_movement"));
    mSplitMode = false;

    if (mCurrentObject != nullptr)
    {
        if (mCurrentFigure != nullptr && mCurrentTemplate != nullptr)
        {
            if (mSplitDepth == false)
            {
                if (mSplitIntersectionsCount == 2)
                {
                    unsigned int fsize = mSplitIntersection1Index +
                                         2 +
                                         mCurrentFigure->EnumVertices() -
                                         mSplitIntersection2Index;

                    CFigureParams fp;
                    fp.mVertices.resize(fsize);
                    fp.mDepth = mCurrentFigure->GetDepth();
                    fp.mZ = mCurrentFigure->GetZ();

                    unsigned int i = 0;

                    for (; i <= mSplitIntersection1Index; i++)
                    {
                        fp.mVertices[i] = mCurrentFigure->GetVertices()[i];
                    }
                    fp.mVertices[i++] = mSplitIntersection1;
                    fp.mVertices[i++] = mSplitIntersection2;
                    for (i = mSplitIntersection2Index + 1; i < mCurrentFigure->EnumVertices(); i++)
                    {
                        fp.mVertices[i] = mCurrentFigure->GetVertices()[i];
                    }

                    mCurrentObject->CreateFigure(fp);

                    fsize = mSplitIntersection2Index -
                            mSplitIntersection1Index +
                            2;

                    fp.mVertices.clear();
                    fp.mVertices.resize(fsize);

                    i = 0;
                    fp.mVertices[i++] = mSplitIntersection1;
                    for (unsigned int j = mSplitIntersection1Index + 1; j <= mSplitIntersection2Index; j++)
                    {
                        fp.mVertices[i++] = mCurrentFigure->GetVertices()[j];
                    }
                    fp.mVertices[i] = mSplitIntersection2;

                    mCurrentObject->CreateFigure(fp);

                    mCurrentObject->DestroyFigure(mCurrentFigure);
                    mCurrentFigure = nullptr;
                    mCurrentObject->DumpGeometry(mCurrentTemplate);

                    GetEventSystem().SetMode(std::string("editor_mode"));
                }
            }
            else
            {
                CFigureParams fp;
                fp.mVertices.resize(mCurrentFigure->EnumVertices());
                memcpy(&fp.mVertices[0], mCurrentFigure->GetVertices(), sizeof(CVec2f) * mCurrentFigure->EnumVertices());

                float min = mCurrentObject->GetPosZ() +
                            mCurrentFigure->GetZ() +
                            0.5f * mCurrentFigure->GetDepth();
                float max = mCurrentObject->GetPosZ() +
                            mCurrentFigure->GetZ() -
                            0.5f * mCurrentFigure->GetDepth();

                fp.mZ = 0.5f * (mSplitFigureCenterZ + min);
                fp.mDepth = math::Abs(mSplitFigureCenterZ - min);
                mCurrentObject->CreateFigure(fp);

                fp.mZ = 0.5f * (mSplitFigureCenterZ + max);
                fp.mDepth = math::Abs(max - mSplitFigureCenterZ);
                mCurrentObject->CreateFigure(fp);

                mCurrentObject->DestroyFigure(mCurrentFigure);
                mCurrentFigure = nullptr;
                mCurrentObject->DumpGeometry(mCurrentTemplate);

                GetEventSystem().SetMode(std::string("editor_mode"));
            }
        }
    }
}

void CTest1::ExtrudeFigure(float _delta_depth)
{
    if (mCurrentFigure != nullptr)
    {
        mCurrentFigure->SetDepth(math::Max<float>(0, mCurrentFigure->GetDepth() + _delta_depth));

        mCurrentObject->DumpGeometry(mCurrentTemplate);
    }
}

void CTest1::CamViewProcessors()
{
    GetEventSystem().SetProcessor("MB", CAppEventProcessor(
    [this] ()
    {
        mCamRotFirstClick = GetCursorPos();
    },
    [this] ()
    {
        CVec2f new_pos = GetCursorPos();

        CVec2f rot = mCamera->GetRotation().Get();
        rot.mY -= new_pos.mX - mCamRotFirstClick.mX;
        rot.mX += new_pos.mY - mCamRotFirstClick.mY;

        mCamera->GetRotation().Set(rot);

        mCamRotFirstClick = new_pos;
    }));

    GetEventSystem().SetProcessor("w", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Forward(50 * GetCurrentTimeDelta());
    }));

    GetEventSystem().SetProcessor("a", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Strafe(50 * GetCurrentTimeDelta());
    }));

    GetEventSystem().SetProcessor("s", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Forward(-50 * GetCurrentTimeDelta());
    }));

    GetEventSystem().SetProcessor("d", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Strafe(-50 * GetCurrentTimeDelta());
    }));

    GetEventSystem().SetProcessor("q", CAppEventProcessor(
    [this] ()
    {
        if (mCurrentTemplate == nullptr)
        {
            mCurrentTemplate = (--GetGeometryManager().GetGeometries().end())->second;

            if (mCurrentObject != nullptr)
            {
                GetScene().DestroyObject(mCurrentObject);
            }

            CSceneObjectParams p;
            p.mDynamic = false;
            mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
            mCurrentObject->SetActive(false);
        }
        else
        {
            auto &t = GetGeometryManager().GetGeometries();
            for (auto i = t.begin(); i != t.end(); i++)
            {
                if (mCurrentTemplate == i->second)
                {
                    if (i == t.begin())
                    {
                        break;
                    }

                    i--;

                    mCurrentTemplate = i->second;

                    if (mCurrentObject != nullptr)
                    {
                        GetScene().DestroyObject(mCurrentObject);
                    }

                    CSceneObjectParams p;
                    p.mDynamic = false;
                    mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
                    mCurrentObject->SetActive(false);

                    break;
                }
            }
        }
    }));
}

void CTest1::CompleteFigure()
{
    if (mCurrentTemplate != nullptr && mVertices.size() >= 3)
    {        
        mCurrentTemplate->mFigures.resize(mCurrentTemplate->mFigures.size()+1);

        mCurrentTemplate->mFigures.back().mVertices = mVertices;

        for (auto i = mCurrentTemplate->mFigures.back().mVertices.begin();
             i != mCurrentTemplate->mFigures.back().mVertices.end(); i++)
        {
            CPlane plane;
            plane.SetNormal(CVec3f(0, 0, 1));
            plane.SetPoint(CVec3f(0, 0, 0));

            CVec3f pos;

            mCamera->CastRay(*i, plane, pos);

            *i = pos;
        }

        if (mCurrentObject != nullptr)
        {
            GetScene().DestroyObject(mCurrentObject);
            this->mCurrentObject = nullptr;
        }

        CSceneObjectParams p;
        p.mDynamic = false;
        mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
        mCurrentObject->SetActive(false);

        mVertices.clear();
    }
}

void CTest1::CreateGeometry()
{
    static const unsigned max_try_count = 10;
    unsigned int counter = 0;

    do
    {
        std::ostringstream is;
        is<<"new_geometry_"<<(mTemplateCounter++);
        this->mCurrentTemplate = GetGeometryManager().CreateGeometry(is.str().c_str());
    }
    while (this->mCurrentTemplate == nullptr && counter < max_try_count);

    if (mCurrentObject != nullptr)
    {
        GetScene().DestroyObject(mCurrentObject);
        this->mCurrentObject = nullptr;
    }
}

void CTest1::DetachCurrentObject()
{
    if (mCurrentObject != nullptr)
    {
        mCurrentObject->SetDynamic(true);
        mCurrentObject->SetActive(true);

        if (mCurrentTemplate != nullptr)
        {
            CSceneObjectParams p;
            p.mDynamic = false;
            mCurrentObject = GetScene().CreateObject(*mCurrentTemplate, p);
            mCurrentObject->SetActive(false);
        }
    }
}

void CTest1::SelectFigure()
{
    mCurrentFigure = GetDebugRenderer().FindFigure(mCamera, GetCursorPos());

    if (mCurrentFigure != nullptr)
    {
        GetEventSystem().SetMode("figure_movement");
    }
    else
    {
        GetEventSystem().SetMode("editor_mode");
    }
}

} // namespace test

} // namespace drash
