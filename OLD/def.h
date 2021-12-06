#ifndef __def_h_
#define __def_h_

#include <freeglut.h>
#include <windows.h>

#include <string>
#include <vector>
#include <map>

#include "mth/mthdef.h"
#include "mth/mth_vec2.h"
#include "mth/mth_vec3.h"
#include "mth/mth_vec4.h"
#include "mth/mth_camera.h"
#include "mth/mth_ray.h"
#include "mth/mth_matr.h"

typedef mth::vec2<FLT> vec2;
typedef mth::vec3<FLT> vec3;
typedef mth::vec4<FLT> vec4;

#pragma warning(disable: 4244)

#endif /* __def_h_ */