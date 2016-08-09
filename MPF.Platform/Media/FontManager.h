//
// MPF Platform
// Font Manager
// 作者：SunnyCase
// 创建时间：2016-08-09
//
#pragma once
#include "../../inc/common.h"
#include <ft2build.h>
#include <freetype/freetype.h>

DEFINE_NS_PLATFORM
#include "../MPF.Platform_i.h"

class FontFace : public WRL::RuntimeClass<WRL::RuntimeClassFlags<WRL::ClassicCom>, IFontFace>
{
public:
	FontFace(FT_Face face);
	virtual ~FontFace();

	// 通过 RuntimeClass 继承
	STDMETHODIMP get_FaceCount(UINT *value) override;
	STDMETHODIMP CreateGlyphGeometry(IResourceManager *resMgr, UINT unicode, IResource **geometry) override;
private:
	FT_Face _face;
};

class FontManager : std::enable_shared_from_this<FontManager>
{
public:
	FontManager();
	~FontManager();

	void CreateFontFaceFromMemory(INT_PTR buffer, UINT64 size, UINT faceIndex, IFontFace** fontFace);
private:
	FT_Library _freetype;
};

END_NS_PLATFORM