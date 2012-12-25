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

#include "templatesystem.h"

#include "../cscene.h"
#include "../sceneobjects/figure.h"
#include "../sceneobjects/csceneobject.h"

namespace drash
{

bool CTemplateSystem::Init()
{
    return true;
}

void CTemplateSystem::Step(double)
{
}

void CTemplateSystem::Release()
{
    for (auto i=mSceneObjectTemplates.begin(), i_e=mSceneObjectTemplates.end(); i!=i_e; i++)
    {
        delete i->second;
    }
    mSceneObjectTemplates.clear();
}

CSceneObjectGeometry *CTemplateSystem::CreateSceneObjectTemplate(const std::string &_name)
{
    if (_name == "" )
    {
        return nullptr;
    }
    else
    {
        if (mSceneObjectTemplates.find(_name) != mSceneObjectTemplates.end()) {
            LOG_ERR("Template " << _name.c_str() << " exists in TemplateSystem");
            return nullptr;
        }
        MapSceneObjectItem elem;
        elem.second = new CSceneObjectGeometry();
        elem.first = _name;
        mSceneObjectTemplates.insert(elem);
        return elem.second;
        //mSceneObjectTemplates.push_back(new CSceneObjectTemplate());
        //mSceneObjectTemplates.back()->mName = _name;
        //return mSceneObjectTemplates.back();
    }
}

void CTemplateSystem::DestoySceneObjectTemplate(CSceneObjectGeometry *_t)
{
    for (auto i=mSceneObjectTemplates.begin(), i_e=mSceneObjectTemplates.end(); i!=i_e; i++)
    {
        if (i->second == _t)
        {
            delete _t;
            mSceneObjectTemplates.erase(i);
            return;
        }
    }
    //    auto obj = mSceneObjectTemplates.find()
}

void CTemplateSystem::RemoveSceneObjectTemplate(const std::string &_name)
{
    auto iter = mSceneObjectTemplates.find(_name);
    if (iter != mSceneObjectTemplates.end())
    {
        delete iter->second;
        mSceneObjectTemplates.erase(iter);
    }
}

void CTemplateSystem::ChangeGeometry(CSceneObjectGeometry *_t, const std::string &_name)
{
    auto iter = mSceneObjectTemplates.find(_name);
    if (iter != mSceneObjectTemplates.end()) {
        delete iter->second;
        iter->second = _t;
    } else {
        LOG_ERR("Template " << _name.c_str() << " not found in TemplateSystem");
    }
}

CSceneObject *CTemplateSystem::CreateSceneObjectFromTemplate(const std::string &_name, const CSceneObjectParams &_params)
{
//    for (auto i=mSceneObjectTemplates.begin(), i_e=mSceneObjectTemplates.end(); i!=i_e; i++)
//    {
//        if ((*i)->mName == _name)
//        {
//            return GetScene()->CreateObject<CSceneObject>((*i)->mGeometry, _params);
//        }
//    }
    auto iter = mSceneObjectTemplates.find(_name);
    if (iter == mSceneObjectTemplates.end()) {
        LOG_ERR("Object" << _name.c_str() << "not found in CTemplateSystem");
        return nullptr;
    }
    return GetScene()->CreateObject<CSceneObject>(*(iter->second), _params);
}

CSceneObjectGeometry *CTemplateSystem::FindTemplate(const std::string &_name)
{
    auto iter = mSceneObjectTemplates.find(_name);
    if (iter == mSceneObjectTemplates.end()) {
        LOG_ERR("Object " << _name.c_str() << " not found in CTemplateSystem");
        return nullptr;
    } else {
        return iter->second;
    }

}

const CTemplateSystem::SceneObjectTemplatesT &CTemplateSystem::GetSceneObjectTemplates() const
{
    return this->mSceneObjectTemplates;
}


void CTemplateSystem::RenameTemplate(const std::string &_oldName, const std::string &_newName)
{
    auto iter = mSceneObjectTemplates.find(_oldName);

    if (iter == mSceneObjectTemplates.end()) {
        return;
    }

    MapSceneObjectItem buff = *iter;
    mSceneObjectTemplates.erase(iter);
    buff.first = _newName;
    mSceneObjectTemplates.insert(buff);
}

}// namespace drash
