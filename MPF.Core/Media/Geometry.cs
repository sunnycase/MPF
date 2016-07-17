using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class Geometry : DependencyObject
    {
        public static readonly Geometry Empty = new Geometry();

        public static readonly DependencyProperty<Matrix3x2> TransformProperty = DependencyProperty.Register(nameof(Transform),
            typeof(Geometry), Matrix3x2.Identity);

        public Matrix3x2 Transform
        {
            get { return GetValue(TransformProperty); }
            set { SetValue(TransformProperty, value); }
        }

        public Geometry()
        {

        }
    }
}
