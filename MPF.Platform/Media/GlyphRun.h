//
// MPF Platform
// Glyph Run
// 作者：SunnyCase
// 创建时间：2016-08-07
//
#pragma once
#include "../../inc/common.h"
#include "ResourceContainer.h"
#include "ResourceRef.h"
#include <ft2build.h>
#include <freetype/freetype.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class GlyphRun
{
public:
	GlyphRun();
private:
	FT_Library _freetype;
};

END_NS_PLATFORM