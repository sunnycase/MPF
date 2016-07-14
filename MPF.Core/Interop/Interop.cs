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
        private static extern int CreateNativeApplication([MarshalAs(UnmanagedType.Interface)]out INativeApplication obj);

        public static INativeApplication CreateNativeApplication()
        {
            INativeApplication obj;
            Marshal.ThrowExceptionForHR(CreateNativeApplication(out obj));
            return obj;
        }
    }

    internal static class Libraries
    {
        public const string Platform = "MPF.Platform.dll";
    }
}
