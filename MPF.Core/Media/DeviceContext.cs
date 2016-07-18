using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class DeviceContext
    {
        private static readonly Lazy<DeviceContext> _current = new Lazy<DeviceContext>(() => new DeviceContext(), true);

        public DeviceContext Current => _current.Value;

        private DeviceContext()
        {

        }
    }
}
