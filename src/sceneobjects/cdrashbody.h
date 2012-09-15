#pragma once
#ifndef CDRASHBODY_H
#define CDRASHBODY_H

#include "csolidbody.h"
#include "cboom.h"
#include <vector>
#include "../diag/ctimer.h"

namespace drash
{

class CDrashBodyParams : public CSolidBodyParams
{
public:
    CDrashBodyParams();

    std::vector<CDrashBodyParams> mChilds;
    CVec2 mLocalPos;
    double mDestroyDelay;
    float mDestroySpeed;
};

class CDrashBody : public CSolidBody
{
public:
    friend class CScene;

    typedef CDrashBodyParams ParamsT;

protected:
    CDrashBody();
    virtual ~CDrashBody();

    virtual bool Init( const ParamsT &_params );

    virtual void Step( double _dt );

    virtual void OnContactBegin( const CContact &_contact );
    virtual void OnBoom( const CBoomParams &_boom );

private:
    int mCounter;
    CVec2 mLastVelocity;
    float mLastAngularVelocity;
    ParamsT mParams;
    CTimer mTimer;
    std::vector<CBoomParams> mBoomParams;
};

} // namespace drash

#endif // CDRASHBODY_H
