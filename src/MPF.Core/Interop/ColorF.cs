using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    [StructLayout(LayoutKind.Sequential, Pack = 4)]
    internal struct ColorF
    {
        public float A;
        public float R;
        public float G;
        public float B;
    }
}
