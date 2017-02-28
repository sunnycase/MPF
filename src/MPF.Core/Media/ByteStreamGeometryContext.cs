using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;
using Segments = MPF.Interop.PathGeometrySegments;

namespace MPF.Media
{
    internal class ByteStreamGeometryContext : IStreamGeometryContext
    {
        private readonly MemoryStream _byteStream = new MemoryStream();

        public ByteStreamGeometryContext()
        {

        }

        #region IDisposable Support
        private bool disposedValue = false; // 要检测冗余调用

        protected void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    CloseOverride(_byteStream.ToArray());
                    _byteStream.Dispose();
                }
                disposedValue = true;
            }
        }

        protected virtual void CloseOverride(byte[] byteStream)
        {

        }

        public void Dispose()
        {
            Dispose(true);
        }

        public unsafe void MoveTo(Point point)
        {
            var seg = new Segments.MoveTo
            {
                Type = Segments.Operations.MoveTo,
                Point = point
            };
            WriteBytes((byte*)&seg, sizeof(Segments.MoveTo));
        }

        public unsafe void LineTo(Point point)
        {
            var seg = new Segments.LineTo
            {
                Type = Segments.Operations.LineTo,
                Point = point
            };
            WriteBytes((byte*)&seg, sizeof(Segments.LineTo));
        }

        public unsafe void ArcTo(Point point, float angle)
        {
            var seg = new Segments.ArcTo
            {
                Type = Segments.Operations.ArcTo,
                Point = point,
                Angle = angle
            };
            WriteBytes((byte*)&seg, sizeof(Segments.ArcTo));
        }

        private unsafe void WriteBytes(byte* data, int size)
        {
            var buffer = new byte[size];
            Marshal.Copy(new IntPtr(data), buffer, 0, size);
            _byteStream.Write(buffer, 0, buffer.Length);
        }
        #endregion
    }
}
