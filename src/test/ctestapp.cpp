#include "ctestapp.h"

#include <GL/gl.h>
#include "ctestapp1.h"
#include "ctestapp2.h"
#include "ctestapp3-drashtest1.h"

namespace drash
{

CTestApp::CTestApp():
    mCamera(NULL),
    mScene(NULL)
{
}

CTestApp::~CTestApp()
{
}

CTestApp *CTestApp::StartApp( const char *_name )
{
    if ( strcmp(_name, "test_basic" ) == 0 ) return new CTestApp;
    if ( strcmp(_name, "test1" ) == 0 ) return new CTestApp1;
    if ( strcmp(_name, "test2") == 0) return new CTestApp2;
    if ( strcmp(_name, "test3") == 0) return new CTestApp3;
    return NULL;
}

bool CTestApp::Init(CScene *_scene, CCamera *_camera )
{
    if ( _camera == NULL )
    {
        return false;
    }
    if ( _scene == NULL )
    {
        return false;
    }

    mCamera = _camera;
    mScene = _scene;

    return true;
}

void CTestApp::Release()
{
    mScene = NULL;
    mCamera = NULL;
}

void CTestApp::Update()
{
}

void CTestApp::Render()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho( -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f );

    float y = ( static_cast<float>( mCamera->GetZoom() - 1 ) / ( mCamera->m_ZoomMax - 1 ) ) * 1.7 - 0.85f;

    glBegin(GL_LINES);
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.85f, -0.9f );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.85f, 0.9f );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.95f, -0.9f );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.75f, -0.9f );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.95f, 0.9f );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.75f, 0.9f );

    glColor3f( 0, 1, 0 );
    glVertex2f( -0.9f, y );
    glColor3f( 0, 1, 0 );
    glVertex2f( -0.8f, y );
    glEnd();
}

CScene *CTestApp::GetScene()
{
    return mScene;
}

CCamera *CTestApp::GetCamera()
{
    return mCamera;
}

} // namespace drash
