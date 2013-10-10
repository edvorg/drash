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

#ifndef CTOUCHLISTENER_H
#define CTOUCHLISTENER_H

#include "touchevent.h"

namespace drash
{

// interface listener event
class CTouchListener
{
public:
    CTouchListener() = default;

public:
    virtual void TouchPress(const CTouchEvent & _evt) = 0;
    virtual void TouchPressing(const CTouchEvent & _evt) = 0;
    virtual void TouchRelease(const CTouchEvent & _evt) = 0;

};


}// namespace drash
#endif // CTOUCHLISTENER_H