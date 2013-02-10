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

#include "sceneeditorapp.h"
#include "../scene/sceneobject.h"
#include "../levelmanager/level.h"
#include "../diag/logger.h"

#include <QDebug>

using namespace greng;
using namespace std;

namespace drash {

CSceneEditorApp::CSceneEditorApp()
{
}

bool CSceneEditorApp::Init()
{

    if (CApp::Init() == false) {
        return false;
    }

    if (InitCamera() == false ||
        InitPointLight() == false ||
        InitSpecialPoint() == false) {
        return false;
    }

    SetProcessors();
    SetCameraProcessors();
    SetDragDropProcessors();
    GetDebugRenderer().SetTexCoordsScale(0.5);
    mTimer.Reset(true);

//    mMoveablePoint.SetSize(100);
    return true;
    //GetTemplateSystem().CreateSceneObjectFromTemplate("Object1",CSceneObjectParams());
}

void CSceneEditorApp::Step(double _dt)
{
    mTimer.Tick();
    if (mPlayLevel == true ){
        CApp::Step(_dt);
    } else {
        CApp::Step(0);
    }
    if (mSelectedObject != nullptr){
        mMoveablePoint.SetCursorPos(GetCursorPos());
        mRotationablePoint.SetCursorPos(GetCursorPos());
        mMoveablePoint.Step(_dt);
        mRotationablePoint.Step(_dt);

        MoveOfAxis();
        RatateObject();
    }
}

void CSceneEditorApp::Render()
{
    CApp::Render();
//    if (mPlayLevel == false && mCurrentLevel != nullptr) {
//        for (const auto &headitem : mCurrentLevel->GetObjects()) {
//            string templatename = headitem.first;
//            CSceneObjectGeometry *g = GetTemplateSystem().FindTemplate(templatename);
//            for (const auto &item : headitem.second) {
//                CSceneObjectParams params = item.second;
//                GetDebugRenderer().RenderObject(g,&params);
//            }
//        }
//    }

    RenderDragTemplate();
    RenderPoints();
}

void CSceneEditorApp::Release()
{
    CApp::Release();
    mRotationablePoint.Release();
}

void CSceneEditorApp::UpdateTemplateSystem()
{
    GetTemplateSystem().Load();
}

bool CSceneEditorApp::LoadLevel(const std::string &_filename)
{
    if (mCurrentLevel != nullptr) {
        GetLevelManager().DestroyLevel(mCurrentLevel);
        mCurrentLevel = nullptr;
    }

    mSelectedObject = nullptr;
    mCurrentLevel = GetLevelManager().CreateLevel();

    if (mCurrentLevel == nullptr) {
        return false;
    }

    if (mCurrentLevel->Load(_filename) == false) {
        return false;
    }

    mFileNameLevel = _filename;

    mTreeRefreshHandler();
    GetLevelManager().StartLevel(mCurrentLevel,mObjectParams);
    mPlayLevel = false;
    return true;
    //GetLevelManager().StartLevel(level);
}

bool CSceneEditorApp::SaveLevelAs(const std::string &_filename)
{
    if (mCurrentLevel == nullptr) {
        LOG_ERR("Level not saved, because not created.");
        return false;
    }

    bool result =  mCurrentLevel->Store(_filename);

    if (result == true) {
        mFileNameLevel = _filename;
    }

    return result;
}

bool CSceneEditorApp::NewLevel()
{
    mSelectedObject = nullptr;
    if (mCurrentLevel != nullptr) {
        GetLevelManager().DestroyLevel(mCurrentLevel);
    }
    mCurrentLevel = GetLevelManager().CreateLevel();
    if (mCurrentLevel == nullptr) {
        return false;
    }
    GetLevelManager().StartLevel(mCurrentLevel);
    mFileNameLevel = "";
    return true;
}

void CSceneEditorApp::StartCurrentLevel()
{
    if (mCurrentLevel != nullptr) {
        mPlayLevel = true;
        mSelectedObject = nullptr;
    }
}

void CSceneEditorApp::AddObject(const std::string &_name, const CVec3f &_pos)
{
    if (mCurrentLevel == nullptr) {
        LOG_WARN("Not set current level");
        return;
    }

    CSceneObjectGeometry *obj = GetTemplateSystem().FindTemplate(_name);
    if (obj == nullptr) {
        LOG_ERR(_name.c_str()  << " not found in Template System");
        return;
    }

    std::ostringstream is;

    is << "object" ;
    if (mCurrentLevel->GetObjects().find(_name) != mCurrentLevel->GetObjects().end()) {
        is << mCurrentLevel->GetObjects().at(_name).size();
    } else {
        is << "0";
    }

    CSceneObjectParams *p = mCurrentLevel->AddObject(_name,is.str());
    if (p == nullptr) {
        return;
    }
    p->mPos = _pos;
    GetLevelManager().StartLevel(mCurrentLevel,mObjectParams);

    mTreeRefreshHandler();
}

void CSceneEditorApp::SetProcessors()
{
    GetEventSystem().SetProcessor("RB",CAppEventProcessor(
    [this]() {
        if (mPlayLevel == true) {
            return;
        }
        mSelectedObject = SelectObject();
        if (mSelectedObject != nullptr) {
            mOldpositon = mSelectedObject->GetPos();
            mMoveablePoint.SetCenter(mOldpositon);
            mRotationablePoint.SetPoint(mOldpositon);
            mRotationablePoint.SetRotation(CVec3f(0,0,mSelectedObject->GetAngle().Get()));
        }
    }
    ));

    GetEventSystem().SetProcessor("LB",CAppEventProcessor(
    [this]() {
        if (mPlayLevel == true) {
            return;
        }
        if (mSelectedObject != nullptr) {
            mMoveablePoint.ClickBegin();
            mRotationablePoint.RotateBegin();
        }
    },
    [this]() {
        if (mPlayLevel == true) {
            return;
        }
        if (mSelectedObject != nullptr) {
            mMoveablePoint.ClickPressing();
        }
    },
    [this] () {
        if (mPlayLevel == true) {
            return;
        }
        if (mSelectedObject != nullptr) {
            mMoveablePoint.ClickEnd();
            mRotationablePoint.RotateEnd();
        }
    }
    ));

    GetEventSystem().SetProcessor("C-LB",CAppEventProcessor(
    [this]() {

        if (mPlayLevel){
            return;
        }
        CSceneObject * temp = SelectObject();
        if (temp != nullptr) {
            auto iter = mObjectParams.find(temp);
            if (iter != mObjectParams.end()) {
                CSceneObjectParams *p = iter->second;
                p->mDynamic = false;
                ResetLevel();
            }
        }
    }
    ));
}

void CSceneEditorApp::SetCameraProcessors()
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
        mCamera->Forward(MOVING_SPEED * mTimer.GetDeltaTime());
    },
    [this]{}
    ));

    GetEventSystem().SetProcessor("a", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Strafe(MOVING_SPEED * mTimer.GetDeltaTime());
    }));

    GetEventSystem().SetProcessor("s", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Forward(-MOVING_SPEED * mTimer.GetDeltaTime());
    }));

    GetEventSystem().SetProcessor("d", CAppEventProcessor(
    [this] () {},
    [this] ()
    {
        mCamera->Strafe(-MOVING_SPEED * mTimer.GetDeltaTime());
    }));
}

void CSceneEditorApp::SetDragDropProcessors()
{
    GetEventSystem().SetProcessor("DRL",CAppEventProcessor(
    [this]() {
        mDragNow = false;
        //mDragTemplateName = "";
    }
    ));

    GetEventSystem().SetProcessor("DRDP",CAppEventProcessor(
    [this]() {
        if (mCurrentLevel != nullptr) {
            //qDebug() << "Proccess enter";
            mDragTemplateName = mGetSelectedTemplateHandler();
            mDragNow = true;
        }

    },
    [this] () {
    },
    [this]() {

        if (mDragNow == true){
            CPlane plane;
            plane.SetNormal(CVec3f(0, 0, 1));
            plane.SetPoint(CVec3f(0, 0, 0));

            CVec3f position;
            CVec2f cpos = GetCursorPos();
            mCamera->CastRay(cpos, plane, position);
            AddObject(mDragTemplateName,position);

            mDragNow = false;
            mDragTemplateName = "";
        }

    }
    ));
}

bool CSceneEditorApp::InitCamera()
{
    greng::CCameraParams p;
    p.mPos.Set(10,10,10.0f);
    p.mRotation.Set(-M_PI/4, M_PI/4, 0);
    mCamera = GetGrengSystems().GetCameraManager().CreateCamera(p);

    if (mCamera == nullptr) {
        return false;
    }

    GetDebugRenderer().SetCamera(mCamera);

    return true;
}

bool CSceneEditorApp::InitPointLight()
{
    mLight1.mPosition.Set(0, 10, 0);
    GetDebugRenderer().SetLight(&mLight1);
    return true;
}

bool CSceneEditorApp::InitSpecialPoint()
{
    mMoveablePoint.SetCamera(mCamera);

    mRotationablePoint.SetRenderer(&GetGrengSystems().GetRenderer());

    mRotationablePoint.SetCamera(mCamera);

    if (mRotationablePoint.Init() == false) {
        return false;
    }

    mRotationablePoint.SetAxisOX(false);
    mRotationablePoint.SetAxisOY(false);

    return true;
}

void CSceneEditorApp::RatateObject()
{

    if (mSelectedObject == nullptr) {
        return;
    }

    auto iter = mObjectParams.find(mSelectedObject);
    if (iter == mObjectParams.end()) {
        return;
    }

    CSceneObjectParams *params = iter->second;
    float anglez = mRotationablePoint.GetRotation().mZ;
    params->mAngle = anglez;
    mSelectedObject->GetAngle().Set(anglez);
    mChangedParams = true;
}

void CSceneEditorApp::RenderPoints()
{
    if (mSelectedObject == nullptr) {
        return;
    }

    mMoveablePoint.Render(GetGrengSystems().GetRenderer());
    mRotationablePoint.Render();
}

void CSceneEditorApp::StoreParams()
{
    if (mCurrentLevel == nullptr) {
        return;
    }
    mObjectParams.clear();

}

void CSceneEditorApp::RenderDragTemplate()
{
    if (mCurrentLevel == nullptr) {
        return;
    }
    if ( mDragNow == false ) {
        return;
    }
    //qDebug() << mDragTemplateName.c_str();
    if (mDragTemplateName != "") {
        CSceneObjectGeometry * g = GetTemplateSystem().FindTemplate(mDragTemplateName);
        if (g == nullptr) {
            return;
        }

        CSceneObjectParams params;
        CVec3f position;
        CVec2f cpos = GetCursorPos();

        CPlane plane;
        plane.SetNormal(CVec3f(0, 0, 1));
        plane.SetPoint(CVec3f(0, 0, 0));

//        LOG_INFO(cpos);
        mCamera->CastRay(cpos, plane, position);
//        v = vv;
        params.mPos = position;
//        LOG_INFO(position);
        GetDebugRenderer().RenderObject(*g,params);
    }
}

CSceneObject *CSceneEditorApp::SelectObject()
{
    return GetDebugRenderer().FindObject(mCamera,GetCursorPos());

    //CVec3f cursorpos;
    //CPlane plane;
    //mCamera->CastRay(GetCursorPos(),);
//    for (const auto &headitem : mCurrentLevel->GetObjects()) {
//        //string templatename = headitem.first;
//        //CSceneObjectGeometry *g = GetTemplateSystem().FindTemplate(templatename);
//        for (const auto &item : headitem.second) {
//            CSceneObjectParams params = item.second;
//            //GetDebugRenderer().RenderObject(g,&params);
//        }
//    }
}

void CSceneEditorApp::MoveOfAxis()
{
    mSelectedObject->SetPos(mMoveablePoint.GetCenter());
    mOldpositon = mMoveablePoint.GetCenter();
    mRotationablePoint.SetPoint(mMoveablePoint.GetCenter());
    auto iter = mObjectParams.find(mSelectedObject);
    if (iter != mObjectParams.end()) {
        CSceneObjectParams *p = iter->second;
        p->mPos = mSelectedObject->GetPos();
    }
}

void CSceneEditorApp::StopLevel()
{
    mPlayLevel = false;
    ResetLevel();
//    GetLevelManager().StartLevel(mCurrentLevel,mObjectParams);
}

void CSceneEditorApp::ResetLevel()
{
    if (mCurrentLevel != nullptr) {
        GetLevelManager().StartLevel(mCurrentLevel,mObjectParams);
    }
}

void CSceneEditorApp::LookObject(const std::string &_templatename, const std::string &_objectname)
{
    if (mCurrentLevel != nullptr) {
        auto headiter = mCurrentLevel->GetObjects().find(_templatename);
        if (headiter != mCurrentLevel->GetObjects().end()) {
            auto iter = headiter->second.find(_objectname);
            if (iter != headiter->second.end()) {
                CSceneObjectParams p = iter->second;
                mCamera->LookAt(p.mPos);
            }
        }
    }
}

void CSceneEditorApp::SetDynamicParam(bool _val)
{
    if (mSelectedObject != nullptr) {
        auto iter = mObjectParams.find(mSelectedObject);
        if (iter != mObjectParams.end()) {
            CSceneObjectParams *temp= iter->second;
            temp->mDynamic = _val;
            CSceneObject *object = iter->first;
            object->SetDynamic(_val);
        }
    }
}

void CSceneEditorApp::SetFixedRotationParam(bool _val)
{
    if (mSelectedObject != nullptr) {
        auto iter = mObjectParams.find(mSelectedObject);
        if (iter != mObjectParams.end()) {
            CSceneObjectParams *temp= iter->second;
            temp->mFixedRotation = _val;
            CSceneObject *object = iter->first;
            object->SetFixedRotation(_val);
        }
    }
}

void CSceneEditorApp::SetAngleParams(float _angle)
{
    if (mSelectedObject != nullptr) {
        auto iter = mObjectParams.find(mSelectedObject);
        if (iter != mObjectParams.end()) {
            CSceneObjectParams *temp= iter->second;
            temp->mAngle = _angle;
            CSceneObject *object = iter->first;
            object->GetAngle().Set(_angle);
        }
    }
}


float CSceneEditorApp::GetAngleParams() const
{
    if (IsObjectSelected() == true) {
        return mSelectedObject->GetAngle().Get();
    }
    return 0.0f;
}


CSceneObjectParams CSceneEditorApp::GetSelectedParams() const
{
    CSceneObjectParams buff;
    if (mSelectedObject != nullptr) {
        auto iter = mObjectParams.find(mSelectedObject);
        if (iter != mObjectParams.end()) {
            CSceneObjectParams *temp= iter->second;
            buff.mAngle = temp->mAngle;
            buff.mDynamic = temp->mDynamic;
            buff.mFixedRotation = temp->mFixedRotation;
            buff.mPos = temp->mPos;
        }
    }
    return buff;
}

} // namspace drash
