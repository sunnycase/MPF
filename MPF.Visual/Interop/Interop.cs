using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    internal static class Platform
    {
        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall)]
        private static extern int CreateNativeWindow([MarshalAs(UnmanagedType.Interface)]out INativeWindow obj);

        public static INativeWindow CreateNativeWindow()
        {
            INativeWindow obj;
            Marshal.ThrowExceptionForHR(CreateNativeWindow(out obj));
            return obj;
        }
    }

    internal static class Libraries
    {
        public const string Platform = "MPF.Platform.dll";
    }
}
