using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Internal.Text
{
    [StructLayout(LayoutKind.Sequential)]
    internal class FontMetrics
    {
        public ushort DesignUnitsPerEm;
        
        public ushort Ascent;
        
        public ushort Descent;
        
        public short LineGap;
        
        public ushort CapHeight;
        
        public ushort XHeight;
        
        public short UnderlinePosition;
        
        public ushort UnderlineThickness;
        
        public short StrikethroughPosition;
        
        public ushort StrikethroughThickness;

        public double LineSpacing
        {
            get
            {
                return (double)(this.LineGap + (short)this.Descent + (short)this.Ascent) / this.DesignUnitsPerEm;
            }
        }

        public double Baseline
        {
            get
            {
                return ((double)this.LineGap * 0.5 + this.Ascent) / this.DesignUnitsPerEm;
            }
        }
    }
}
