using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public abstract class Brush : Animatable, IResourceProvider
    {
        IResource IResourceProvider.Resource => GetResourceOverride();

        internal abstract IResource GetResourceOverride();

        public static implicit operator Brush(Color color)
        {
            return (SolidColorBrush)(color);
        }
    }
}
