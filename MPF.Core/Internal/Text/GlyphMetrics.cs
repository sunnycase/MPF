using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Internal.Text
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct GlyphMetrics
    {
        public int LeftSideBearing;
        
        public uint AdvanceWidth;
        
        public int RightSideBearing;
        
        public int TopSideBearing;
        
        public uint AdvanceHeight;
        
        public int BottomSideBearing;
        
        public int VerticalOriginY;
    }
}
