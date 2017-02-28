using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Input
{
    public class MouseDevice : InputDevice
    {
        internal MouseDevice(IInputDevice device)
            :base(device)
        {

        }
    }
}
