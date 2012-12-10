#include "uisystem.h"

#include "uicontrol.h"

namespace drash
{

bool CUISystem::Init()
{
    return true;
}

void CUISystem::Release()
{
    for (unsigned int i = 0; i < mControlsCount; i++)
    {
        delete mControls[i];
        mControls[i] = nullptr;
    }
    mControlsCount = 0;
}

CUIControl *CUISystem::CreateControl()
{
    if (mControlsCount == mControlsCountLimit)
    {
        return nullptr;
    }

    return mControls[mControlsCount++] = new CUIControl;
}

void CUISystem::DestroyControl(CUIControl *_control)
{
    for (unsigned int i = 0; i < mControlsCount; i++)
    {
        if (mControls[i] == _control)
        {
            _control->mDestroyHandler();
            delete _control;
            mControls[i] = nullptr;
            mControlsCount--;
            return;
        }
    }
}

void CUISystem::SetAspectRatio(float _ratio)
{
    mAspectRatio = _ratio;
    SetWidth(mWidth);
}

void CUISystem::SetWidth(unsigned int _width)
{
    mWidth = _width;
    mHeight = mWidth / mAspectRatio;
}

} // namespace drash
