using MPF.Input;
using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace MPF.Threading
{
    public abstract class DispatcherObject
    {
        private readonly Thread _thread;

        public DispatcherObject()
        {
            _thread = Thread.CurrentThread;
        }

        public void VerifyAccess()
        {
            if (!CheckAccess())
                throw new InvalidOperationException("Dispatcher object must be accessed in created thread.");
        }

        public bool CheckAccess()
        {
            return _thread == Thread.CurrentThread;
        }
    }
}
