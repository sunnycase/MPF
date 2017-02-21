using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media
{
    public abstract class ImageSource : Animatable, IResourceProvider
    {
        internal ImageSource()
        {

        }

        IResource IResourceProvider.Resource => GetTextureResourceOverride();

        internal abstract IResource GetTextureResourceOverride();
    }
}
