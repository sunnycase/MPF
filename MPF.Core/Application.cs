using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public class Application
    {
        private readonly INativeApplication _nativeApp;

        public Application()
        {
            _nativeApp = Platform.CreateNativeApplication();
        }

        public void Run()
        {
            _nativeApp.Run();
        }
    }
}
