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
        private static extern int CreateNativeWindow([MarshalAs(UnmanagedType.FunctionPtr), In]NativeWindowMessageHandler handler, [MarshalAs(UnmanagedType.Interface)]out INativeWindow obj);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall)]
        private static extern int CreateDeviceContext([MarshalAs(UnmanagedType.FunctionPtr), In]DeviceContextMessageHandler messageHandler, [MarshalAs(UnmanagedType.Interface)]out IDeviceContext obj);

        public static INativeApplication CreateNativeApplication()
        {
            INativeApplication obj;
            Marshal.ThrowExceptionForHR(CreateNativeApplication(out obj));
            return obj;
        }

        public static INativeWindow CreateNativeWindow(NativeWindowMessageHandler handler)
        {
            INativeWindow obj;
            Marshal.ThrowExceptionForHR(CreateNativeWindow(handler, out obj));
            return obj;
        }

        public static IDeviceContext CreateDeviceContext(DeviceContextMessageHandler messageHandler)
        {
            IDeviceContext obj;
            Marshal.ThrowExceptionForHR(CreateDeviceContext(messageHandler, out obj));
            return obj;
        }
    }

    internal static class Libraries
    {
        public const string Platform = "MPF.Platform.dll";
    }
}
