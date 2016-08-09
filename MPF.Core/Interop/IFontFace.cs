using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [ComImport]
    [Guid("E9B540D3-DF9B-4619-9251-886AC2B86646")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IFontFace
    {
        uint FaceCount { get; }
        IResource CreateGlyphGeometry(IResourceManager resMgr, uint code);
    }
}
