#include "ctestapp1.h"

#include "../cscene.h"
#include "../diag/ctimer.h"

namespace drash
{

CTimer t;

bool CTestApp1::Init()
{
    if ( CTestApp::Init() == false )
    {
        return false;
    }

//    t.Reset(true);

//    CSceneObjectParams p;
//    p.mFigures.resize(1);
//    p.mFigures[0].mLayers.Set(-500, 500);
//    p.mFigures[0].mVertices.push_back( CVec2( -100.0f, 5.0f ) );
//    p.mFigures[0].mVertices.push_back( CVec2( -100.0f, -5.0f ) );
//    p.mFigures[0].mVertices.push_back( CVec2( 100.0f, -5.0f ) );
//    p.mFigures[0].mVertices.push_back( CVec2( 100.0f, 5.0f ) );
//    p.mPos.y = -25;
//    p.mDynamic = false;
//    GetScene().CreateObject<CSceneObject>(p);

    return true;
}

void CTestApp1::Update()
{
    CTestApp::Update();

//    t.Tick();
//    if ( t.GetFullTime() > 1 )
//    {
//        CDrashBodyParams p;
//        p.mPos.RandY(100, 200, 15);
//        p.mPos.RandX(-50, 50, 15);
//        p.mFigures.resize(1);
//        p.mFigures[0].mLayers.Set(-500, 500);
//        GetScene().CreateObject<CSceneObject>(p);
//        t.Reset(true);
//    }
}

} // namespace drash
