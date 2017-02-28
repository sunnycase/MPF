using MPF.Interop;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;

namespace MPF.Input
{
    public class InputDevice
    {
        internal readonly IInputDevice _device;

        public HIDUsage HIDUsage => _device.HIDUsage;

        internal InputDevice(IInputDevice device)
        {
            _device = device;
        }

        private static readonly ConcurrentDictionary<IInputDevice, InputDevice> _inputDevices = new ConcurrentDictionary<IInputDevice, InputDevice>();

        static internal InputDevice GetOrAddDevice(IInputDevice device)
        {
            return _inputDevices.GetOrAdd(device, d =>
            {
                var usage = d.HIDUsage;
                if (usage == HIDUsages.GenericDesktop.Mouse)
                    return new MouseDevice(d);
                return new InputDevice(d);
            });
        }
    }
}
