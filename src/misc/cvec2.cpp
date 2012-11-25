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

#include "cvec2.h"

namespace drash
{

const float CVec2::mAccuracy = 0.001;

float Randf( float _min, float _max, float _step )
{
    return ::rand() %
            static_cast<int>( ( _max - _min ) / _step + 1 ) *
            _step +
            _min;
}

CVec2::CVec2():
    b2Vec2( 0.0f, 0.0f )
{
}

CVec2::CVec2( float32 _val ):
    b2Vec2( _val, _val )
{
}

CVec2::CVec2( float32 _x, float32 _y ):
    b2Vec2( _x, _y )
{
}

CVec2::CVec2( const b2Vec2 &_copy )
{
    this->x = _copy.x;
    this->y = _copy.y;
}

CVec2 &CVec2::Rand( float _min, float _max, float _step ) //const
{
    x = drash::Randf( _min, _max, _step );
    y = drash::Randf( _min, _max, _step );
    return *this;
}

CVec2 &CVec2::RandX( float _min, float _max, float _step )
{
    x = drash::Randf( _min, _max, _step );
    return *this;
}

CVec2 &CVec2::RandY( float _min, float _max, float _step )
{
    y = drash::Randf( _min, _max, _step );
    return *this;
}

CVec2 &CVec2::operator=( const b2Vec2 &_ver )
{
    this->x = _ver.x;
    this->y = _ver.y;
    return (*this);
}

CVec2 &CVec2::operator*=( const b2Vec2 &_ver )
{
    this->x = x*_ver.x;
    this->y = y*_ver.y;
    return *this;
}

CVec2 &CVec2::operator *=( float _val )
{
    x *= _val;
    y *= _val;
    return (*this);
}

CVec2 &CVec2::operator +=( float _val )
{
    x += _val;
    y += _val;
    return (*this);
}

CVec2 &CVec2::operator +=( const b2Vec2 &_ver )
{
    x += _ver.x;
    y += _ver.y;
    return (*this);
}

CVec2 &CVec2::operator -=( float _val )
{
    x -= _val;
    y -= _val;
    return (*this);
}

CVec2 &CVec2::operator -=( const b2Vec2 &_ver )
{
    x -= _ver.x;
    y -= _ver.y;
    return (*this);
}

CVec2 &CVec2::operator /=( float _val )
{
    x /= _val;
    y /= _val;
    return (*this);
}

CVec2 &CVec2::operator /=( const b2Vec2 &_ver )
{
    x /= _ver.x;
    y /= _ver.y;
    return (*this);
}

bool CVec2::operator ==( const CVec2 &_var )
{
    if ( fabs(x - _var.x) < mAccuracy )
    {
        if ( fabs(y - _var.y) < mAccuracy )
        {
            return true;
        }
    }
    return false;
}

float CVec2::Dot( const b2Vec2 &_ver ) const
{
    return x*_ver.x + y*_ver.y;
}

CLogger& operator<<( CLogger& _logger, const b2Vec2 &_v )
{
    _logger<<'('<<_v.x<<"; "<<_v.y<<')';
    return _logger;
}

} // namespace drash
