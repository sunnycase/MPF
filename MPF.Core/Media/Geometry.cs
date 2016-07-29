using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using MPF.Interop;

namespace MPF.Media
{
    public abstract class Geometry : Animatable, IResourceProvider
    {
        public static readonly Geometry Empty = new StreamGeometry();

        public static readonly DependencyProperty<Matrix3x2> TransformProperty = DependencyProperty.Register(nameof(Transform),
            typeof(Geometry), Matrix3x2.Identity);

        public Matrix3x2 Transform
        {
            get { return GetValue(TransformProperty); }
            set { SetValue(TransformProperty, value); }
        }

        IResource IResourceProvider.Resource => GetResourceOverride();

        internal Geometry()
        {

        }

        internal abstract IResource GetResourceOverride();
    }
}
