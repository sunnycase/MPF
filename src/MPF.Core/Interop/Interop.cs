using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace MPF.Interop
{
    internal static class Platform
    {
        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall, PreserveSig = false)]
        private static extern void CreateNativeApplication([MarshalAs(UnmanagedType.Interface)]out INativeApplication obj);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall, PreserveSig = false)]
        private static extern void CreateNativeWindow([MarshalAs(UnmanagedType.Interface), In]INativeWindowCallback callback, [MarshalAs(UnmanagedType.Interface)]out INativeWindow obj);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall, PreserveSig = false)]
        private static extern void CreateDeviceContext(RenderBackendType preferredBackend, [MarshalAs(UnmanagedType.Interface), In]IDeviceContextCallback callback, [MarshalAs(UnmanagedType.Interface)]out IDeviceContext obj);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall, PreserveSig = false)]
        private static extern void GetSystemFontFaceLocation([MarshalAs(UnmanagedType.BStr), In]string faceName, [MarshalAs(UnmanagedType.BStr), Out]out string location);

        [DllImport(Libraries.Platform, CallingConvention = CallingConvention.StdCall, PreserveSig = false)]
        private static extern void GetCurrentInputManager([MarshalAs(UnmanagedType.Interface)]out IInputManager obj);

        public static INativeApplication CreateNativeApplication()
        {
            INativeApplication obj;
            CreateNativeApplication(out obj);
            return obj;
        }

        public static INativeWindow CreateNativeWindow(INativeWindowCallback callback)
        {
            INativeWindow obj;
            CreateNativeWindow(callback, out obj);
            return obj;
        }

        public static IDeviceContext CreateDeviceContext(RenderBackendType prefferedBackend, IDeviceContextCallback callback)
        {
            IDeviceContext obj;
            CreateDeviceContext(prefferedBackend, callback, out obj);
            return obj;
        }

        public static Uri GetSystemFontFaceLocation(string faceName)
        {
            string location;
            GetSystemFontFaceLocation(faceName, out location);
            return new Uri(location, UriKind.Absolute);
        }

        public static IInputManager GetCurrentInputManager()
        {
            IInputManager obj;
            GetCurrentInputManager(out obj);
            return obj;
        }
    }

    internal static class Libraries
    {
        public const string Platform = "MPF.Platform.dll";
    }
}
