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

#include "test4.h"

#include "../ui/uicontrol.h"

namespace drash
{

namespace test
{

bool CTest4::Init()
{
    if (CApp::Init() == false)
    {
        return false;
    }

    mTestButton1.Connect(&GetUISystem());

    mTestButton1.SetClickHandler([] ()
    {
        LOG_INFO("click 1");
    });

    mTestButton1.SetPos(CVec2i(100, 100));
    mTestButton1.SetSize(CVec2ui(200, 30));

    mTestButton2.Connect(&GetUISystem());

    mTestButton2.SetClickHandler([] ()
    {
        LOG_INFO("click 2");
    });

    mTestButton2.SetPos(CVec2i(350, 100));
    mTestButton2.SetSize(CVec2ui(30, 30));

    mTestSlider1.Connect(&GetUISystem());

    mTestSlider1.SetPos(CVec2i(430, 100));
    mTestSlider1.SetSize(CVec2ui(200, 30));

    mTestSlider1.SetMin(-100);
    mTestSlider1.SetMax(200);
    mTestSlider1.SetValue(5);
    mTestSlider1.SetSliderWidth(100);

    mTestSlider1.SetValueHandler([] (float _value)
    {
        LOG_INFO(_value);
    });

    LOG_INFO("value is "<<mValueAnimator);
    mValue = 33;
    LOG_INFO("value is "<<mValueAnimator);

    mValueAnimator.SetTarget(100, 1.0f, AnimatorBehavior::Bounce);

    return true;
}

void CTest4::Step(double _dt)
{
    CApp::Step(_dt);

    if (mValueAnimator.Step(_dt))
    {
        mTestSlider1.SetPercent(mValueAnimator / 100.0);
    }
}

} // namespace test

} // namespace drash
