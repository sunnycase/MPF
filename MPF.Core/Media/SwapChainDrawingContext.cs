using MPF.Interop;
using MPF.Media3D;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media
{
    struct SwapChainDrawingContext
    {
        private readonly ISwapChainDrawingContext _context;

        public SwapChainDrawingContext(ISwapChainDrawingContext context)
        {
            _context = context;
        }

        public void PushViewport(ref Rect viewport)
        {
            _context.PushViewport(ref viewport);
        }

        public void PopViewport()
        {
            _context.PopViewport();
        }

        public void PushCamera(Camera camera)
        {
            _context.PushCamera(((IResourceProvider)camera).Resource);
        }

        public void PopCamera()
        {
            _context.PopCamera();
        }
    }
}
