using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;
using MPF.Interop;
using System.Runtime.InteropServices;

namespace MPF.Media
{
    public abstract class Geometry : Animatable, IResourceProvider, IDisposable
    {
        public static readonly Geometry Empty = new StreamGeometry();

        public static readonly DependencyProperty<Matrix3x2> TransformProperty = DependencyProperty.Register(nameof(Transform),
            typeof(Geometry), new PropertyMetadata<Matrix3x2>(Matrix3x2.Identity));

        public Matrix3x2 Transform
        {
            get { return GetValue(TransformProperty); }
            set { SetValue(TransformProperty, value); }
        }

        IResource IResourceProvider.Resource => GetResourceOverride();

        internal Geometry()
        {
            GC.AddMemoryPressure(512);
        }

        internal abstract IResource GetResourceOverride();

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                var res = GetResourceOverride();
                if (res != null)
                    Marshal.ReleaseComObject(res);
                GC.RemoveMemoryPressure(512);
                disposedValue = true;
            }
        }
        
         ~Geometry()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion
    }
}
