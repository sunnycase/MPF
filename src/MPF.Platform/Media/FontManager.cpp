//
// MPF Platform
// Font Manager
// 作者：SunnyCase
// 创建时间：2016-08-09
//
#include "stdafx.h"
#include "FontManager.h"
#include "ResourceManagerBase.h"
#include "Geometry.h"
#include <freetype/ftoutln.h>
using namespace WRL;
using namespace NS_PLATFORM;

FontManager::FontManager()
{
	ThrowIfNot(FT_Init_FreeType(&_freetype) == 0, L"Cannot initialize Freetype.");
}

FontManager::~FontManager()
{
	FT_Done_FreeType(_freetype);
}

void FontManager::CreateFontFaceFromMemory(INT_PTR buffer, UINT64 size, UINT faceIndex, IFontFace ** fontFace)
{
	FT_Face face;
	ThrowIfNot(FT_New_Memory_Face(_freetype, reinterpret_cast<const FT_Byte*>(buffer), FT_Long(size), faceIndex, &face) == 0,
		L"Cannot load face.");
	*fontFace = Make<FontFace>(face).Detach();
}

FontFace::FontFace(FT_Face face)
	:_face(face)
{
	if (face->face_flags & FT_FACE_FLAG_TRICKY)
		ThrowAlways(L"Tricky font.");
}

FontFace::~FontFace()
{
	FT_Done_Face(_face);
}

HRESULT FontFace::get_FaceCount(UINT *value)
{
	try
	{
		*value = UINT(_face->num_faces);
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT FontFace::get_FontMetrics(FontMetrics *value)
{
	try
	{
		value->Ascent = _face->ascender;
		value->Descent = -_face->descender;
		value->DesignUnitsPerEM = _face->units_per_EM;
		return S_OK;
	}
	CATCH_ALL();
}

HRESULT FontFace::get_UnitsPerEM(UINT *value)
{
	try
	{
		*value = _face->units_per_EM;
		return S_OK;
	}
	CATCH_ALL();
}

namespace
{
	struct OutlineDecomposeContext
	{
		std::vector<PathGeometrySegments::Segment> segments;
	};

	int MoveTo(const FT_Vector* to, void* user)
	{
		try
		{
			using namespace PathGeometrySegments;
			auto& context = *reinterpret_cast<OutlineDecomposeContext*>(user);

			Segment segment;
			segment.Operation = Operations::MoveTo;
			segment.Data.MoveTo.Point = { float(to->x), float(to->y) };
			context.segments.emplace_back(std::move(segment));
			return 0;
		}
		catch (...)
		{
			return -1;
		}
	}

	int LineTo(const FT_Vector* to, void* user)
	{
		try
		{
			using namespace PathGeometrySegments;
			auto& context = *reinterpret_cast<OutlineDecomposeContext*>(user);

			Segment segment;
			segment.Operation = Operations::LineTo;
			segment.Data.LineTo.Point = { float(to->x), float(to->y) };
			context.segments.emplace_back(std::move(segment));
			return 0;
		}
		catch (...)
		{
			return -1;
		}
	}

	int QudraticBezierTo(const FT_Vector* control, const FT_Vector* to, void* user)
	{
		try
		{
			using namespace PathGeometrySegments;
			auto& context = *reinterpret_cast<OutlineDecomposeContext*>(user);

			Segment segment;
			segment.Operation = Operations::QuadraticBezierTo;
			segment.Data.QuadraticBezierTo.Control = { float(control->x), float(control->y) };
			segment.Data.QuadraticBezierTo.Point = { float(to->x), float(to->y) };
			context.segments.emplace_back(std::move(segment));
			return 0;
		}
		catch (...)
		{
			return -1;
		}
	}

	int CubicBezierTo(const FT_Vector* control1, const FT_Vector* control2, const FT_Vector* to, void* user)
	{
		assert(false && "Not Implemented.");
		return 0;
	}

	FT_Outline_Funcs OutlineDecomposeFuncs =
	{
		MoveTo,
		LineTo,
		QudraticBezierTo,
		CubicBezierTo,
		0, 0
	};
}

HRESULT FontFace::CreateGlyphGeometry(IResourceManager *resMgr, UINT unicode, GlyphMetrics* metrics, IResource **geometry)
{
	try
	{
		auto id = FT_Get_Char_Index(_face, unicode);
		ThrowIfNot(id, L"Cannot find char.");
		ThrowIfNot(FT_Load_Glyph(_face, id, FT_LOAD_NO_SCALE) == 0, L"Cannot load glyph.");

		const auto& srcMtcs = _face->glyph->metrics;
		metrics->AdvanceWidth = UINT32(srcMtcs.horiAdvance);
		metrics->AdvanceHeight = UINT32(srcMtcs.vertAdvance);
		metrics->LeftSideBearing = srcMtcs.vertBearingX;
		metrics->BottomSideBearing = srcMtcs.vertBearingY;
		metrics->RightSideBearing = srcMtcs.horiBearingX;
		metrics->TopSideBearing = srcMtcs.horiBearingY;

		OutlineDecomposeContext context;
		ThrowIfNot(FT_Outline_Decompose(&_face->glyph->outline, &OutlineDecomposeFuncs, &context) == 0, L"Cannot decompose outline.");
		ComPtr<IResource> resource;
		ThrowIfFailed(resMgr->CreateResource(RT_PathGeometry, &resource));
		ThrowIfFailed(static_cast<ResourceManagerBase*>(resMgr)->UpdatePathGeometry(resource.Get(), std::move(context.segments)));
		*geometry = resource.Detach();
		return S_OK;
	}
	CATCH_ALL();
}