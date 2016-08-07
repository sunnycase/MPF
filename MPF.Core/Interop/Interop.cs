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

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall)]
        private static extern int CreateNativeWindow([MarshalAs(UnmanagedType.Interface), In]INativeWindowCallback callback, [MarshalAs(UnmanagedType.Interface)]out INativeWindow obj);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall)]
        private static extern int CreateDeviceContext(RenderBackendType preferredBackend, [MarshalAs(UnmanagedType.Interface), In]IDeviceContextCallback callback, [MarshalAs(UnmanagedType.Interface)]out IDeviceContext obj);

        public static INativeApplication CreateNativeApplication()
        {
            INativeApplication obj;
            Marshal.ThrowExceptionForHR(CreateNativeApplication(out obj));
            return obj;
        }

        public static INativeWindow CreateNativeWindow(INativeWindowCallback callback)
        {
            INativeWindow obj;
            Marshal.ThrowExceptionForHR(CreateNativeWindow(callback, out obj));
            return obj;
        }

        public static IDeviceContext CreateDeviceContext(RenderBackendType prefferedBackend, IDeviceContextCallback callback)
        {
            IDeviceContext obj;
            Marshal.ThrowExceptionForHR(CreateDeviceContext(prefferedBackend, callback, out obj));
            return obj;
        }
    }

    internal static class Libraries
    {
        public const string Platform = "MPF.Platform.dll";
    }
}
