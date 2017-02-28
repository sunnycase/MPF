using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal interface IResourceProvider
    {
        IResource Resource { get; }
    }
}
