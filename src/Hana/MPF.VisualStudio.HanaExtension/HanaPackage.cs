using Microsoft.VisualStudio.Shell;
using Microsoft.VisualStudio.Shell.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace MPF.VisualStudio.HanaExtension
{
    [PackageRegistration(UseManagedResourcesOnly = true)]
    [InstalledProductRegistration("#110", "#112", "1.0", IconResourceID = 400)]
    [Guid(HanaPackage.PackageGuidString)]
    public sealed class HanaPackage : Package
    {
        public const string PackageGuidString = "B81D6043-3CBF-4319-A66B-04B24AADAE28";
        private const string CSharpPackageIdString = "13c3bbb4-f18f-4111-9f54-a0fb010d9194";

        protected override void Initialize()
        {
            base.Initialize();

            // We need to force the CSharp package to load. That's responsible for the initialization
            // of the remote host client.
            var shell = GetService(typeof(SVsShell)) as IVsShell;
            if (shell == null)
            {
                return;
            }

            IVsPackage package = null;
            var packageGuid = new Guid(CSharpPackageIdString);
            shell.LoadPackage(ref packageGuid, out package);
        }
    }
}
