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

#ifndef DRASH_OBJECTFACTORY_H
#define DRASH_OBJECTFACTORY_H

#include "../diag/logger.h"

namespace drash {

    template <class T> class ObjectFactory final {
    public:
        class FactoryProduct {
        public:
            friend class ObjectFactory;

        protected:
        private:
            long int internalId;
        };

        ObjectFactory() = delete;
        ObjectFactory(const ObjectFactory&) = delete;
        ObjectFactory(ObjectFactory&&) = delete;
        ObjectFactory& operator=(const ObjectFactory&) = delete;
        ObjectFactory& operator=(ObjectFactory&&) = delete;

        explicit ObjectFactory(unsigned int _size, const char* _product_name);
        ~ObjectFactory();

        T* CreateObject();
        bool DestroyObjectSafe(T* _obj);
        void DestroyObject(T* _obj);
        void DestroyObjects();
        inline bool IsObject(T* _obj) const;
        inline T* const* GetObjects() const;
        inline unsigned int EnumObjects() const;

    protected:
    private:
        T** objects = nullptr;
        unsigned int objectsCount = 0;
        unsigned int objectsCountLimit = 0;
        std::string productName;
    };

    template <class T>
    ObjectFactory<T>::ObjectFactory(unsigned int _size,
                                    const char* _product_name)
        : objectsCountLimit(_size), productName(_product_name) {
        if (objectsCountLimit == 0) {
            LOG_ERR("ObjectFactory<"
                    << productName.c_str()
                    << ">::ObjectFactory(): Empty factory is unusable");
            return;
        }

        objects = new T* [objectsCountLimit];
        for (unsigned int i = 0; i < objectsCount; i++) {
            objects[i] = nullptr;
        }
    }

    template <class T> ObjectFactory<T>::~ObjectFactory() {
        if (objectsCount != 0) {
            LOG_WARN("ObjectFactory<" << productName.c_str()
                                      << ">::~ObjectFactory(): Potential "
                                         "memory leak. You mush remove "
                                         "objects manually");

            for (unsigned int i = 0; i < objectsCount; i++) {
                DestroyObject(objects[i]);
                objects[i] = nullptr;
            }
        }

        delete[] objects;
        objects = nullptr;
    }

    template <class T> T* ObjectFactory<T>::CreateObject() {
        if (objectsCount >= objectsCountLimit) {
            LOG_ERR("ObjectFactory<"
                    << productName.c_str()
                    << ">::CreateObject(): Objects count exceedes it's limit ("
                    << objectsCountLimit << ')');
            return nullptr;
        }

        T* res = objects[objectsCount] = new T();
        res->internalId = objectsCount++;
        return res;
    }

    template <class T> bool ObjectFactory<T>::DestroyObjectSafe(T* _obj) {
        if (IsObject(_obj) == false) {
            LOG_ERR("ObjectFactory<" << productName.c_str()
                                     << ">::DestroyObject(): Something wrong "
                                        "with objects creation logic. "
                                        "Skipping");
            return false;
        }

        DestroyObject(_obj);

        return true;
    }

    template <class T> void ObjectFactory<T>::DestroyObject(T* _obj) {
        if (_obj->internalId < --objectsCount) {
            objects[_obj->internalId] = objects[objectsCount];
            objects[_obj->internalId]->internalId = _obj->internalId;
            objects[objectsCount] = nullptr;
        } else {
            objects[_obj->internalId] = nullptr;
        }

        delete _obj;
    }

    template <class T> void ObjectFactory<T>::DestroyObjects() {
        for (unsigned int i = 0; i < objectsCount; i++) {
            delete objects[i];
            objects[i] = nullptr;
        }

        objectsCount = 0;
    }

    template <class T> inline bool ObjectFactory<T>::IsObject(T* _obj) const {
        return _obj != nullptr && _obj->internalId < objectsCount &&
               _obj == objects[_obj->internalId];
    }

    template <class T> inline T* const* ObjectFactory<T>::GetObjects() const {
        return objects;
    }

    template <class T>
    inline unsigned int ObjectFactory<T>::EnumObjects() const {
        return objectsCount;
    }

} // namespace drash

#endif // DRASH_OBJECTFACTORY_H
