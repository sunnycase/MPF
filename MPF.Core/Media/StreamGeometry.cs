using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MPF.Interop;

namespace MPF.Media
{
    public class StreamGeometry : Geometry
    {
        private readonly Lazy<IResource> _geometryRes;
        private volatile byte[] _pathByteStream;

        public StreamGeometry()
        {
            _geometryRes = this.CreateResource(ResourceType.RT_PathGeometry);
            RegisterUpdateResource();
        }

        internal override IResource GetResourceOverride()
        {
            return _geometryRes.Value;
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdatePathGeometry(_geometryRes.Value, _pathByteStream);
        }

        public IStreamGeometryContext Open()
        {
            return new ByteStreamGeometryContextCallback(this);
        }

        private void Close(byte[] byteStream)
        {
            _pathByteStream = byteStream;
            RegisterUpdateResource();
        }

        class ByteStreamGeometryContextCallback : ByteStreamGeometryContext
        {
            private readonly StreamGeometry _streamGeometry;
            public ByteStreamGeometryContextCallback(StreamGeometry streamGeometry)
            {
                _streamGeometry = streamGeometry;
            }

            protected override void CloseOverride(byte[] byteStream)
            {
                _streamGeometry.Close(byteStream);
                base.CloseOverride(byteStream);
            }
        }
    }
}
