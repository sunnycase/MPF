using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Media3D
{
    public abstract class Geometry3D : Animatable, IResourceProvider, IDisposable
    {
        public static readonly Geometry3D Empty = null;

        IResource IResourceProvider.Resource => GetResourceOverride();

        internal Geometry3D()
        {
            GC.AddMemoryPressure(512);
        }

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

        ~Geometry3D()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        #endregion

        internal abstract IResource GetResourceOverride();
    }
}
