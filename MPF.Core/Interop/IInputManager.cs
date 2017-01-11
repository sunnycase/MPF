using MPF.Input;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Interop
{
    [StructLayout(LayoutKind.Sequential)]
    internal struct HIDMouseInput
    {
        public uint Time;
        public int CursorX;
        public int CursorY;
        public ushort ChangedButton;
        public MouseButtonState ChangedButtonState;
        public short WheelDelta;
        public int XDelta;
        public int YDelta;
    }

    [ComImport]
    [Guid("5867067E-36C7-4702-89B1-5D55AA2C2A88")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IInputManagerCallback
    {
        void OnMouseInput([In]IInputDevice device, [In]ref HIDMouseInput input);
    }

    [ComImport]
    [Guid("3A52BA5A-39BD-4D73-AC2A-3A8BB9B55A20")]
    [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
    internal interface IInputManager
    {
        void SetCallback([In]IInputManagerCallback callback);
        void SetHIDAware([In, MarshalAs(UnmanagedType.LPArray, SizeParamIndex = 1)] HIDUsage[] usages, int length);
    }
}
