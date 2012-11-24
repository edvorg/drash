#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "cvec2.h"
#include "math.h"

namespace drash
{

extern const float g_LayerWidth;

void DrawBodySide(const b2Vec2 &_v1,
              const b2Vec2 &_v2,
              float _z,
              float _depth,
              const b2Color &_diffuse );

void DrawBody(const b2Vec2 *_vertices,
              unsigned int _count,
              float _z,
              float _depth,
              const b2Color &_color);

void DrawCircle(float _rad,
                float _r, float _g, float _b, float _a);

}// namespace drash

#endif // GRAPHICS_H
