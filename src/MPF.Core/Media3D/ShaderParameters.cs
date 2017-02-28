using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.IO;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Media3D
{
    public abstract class ShaderParameters : Animatable, IResourceProvider
    {
        private readonly Lazy<IResource> _shaderParamsRes;

        IResource IResourceProvider.Resource => _shaderParamsRes.Value;

        public ShaderParameters()
        {
            _shaderParamsRes = this.CreateResource(ResourceType.RT_ShaderParameters);
            RegisterUpdateResource();
        }

        protected override void OnDependencyPropertyChanged<T>(PropertyChangedEventArgs<T> args)
        {
            RaiseUpdate();
        }

        protected void RaiseUpdate()
        {
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            using (var context = new ShaderParametersWriteContext())
            {
                OnWriteParameters(context);
                (var data, var brushes) = context.Close();
                MediaResourceManager.Current.UpdateShaderParameters(_shaderParamsRes.Value,
                    data, brushes);
            }
        }

        protected abstract void OnWriteParameters(ShaderParametersWriteContext context);
    }

    public sealed class ShaderParametersWriteContext : IDisposable
    {
        private readonly MemoryStream _stream = new MemoryStream();
        private readonly BinaryWriter _writer;
        private readonly List<IResource> _brushes = new List<IResource>();

        public ShaderParametersWriteContext()
        {
            _writer = new BinaryWriter(_stream);
        }

        public void Write(float value) => _writer.Write(value);

        public void Write(Vector2 value)
        {
            _writer.Write(value.X);
            _writer.Write(value.Y);
        }

        public void Write(Vector3 value)
        {
            _writer.Write(value.X);
            _writer.Write(value.Y);
            _writer.Write(value.Z);
        }

        public void Write(Matrix4x4 value)
        {
            _writer.Write(value.M11);
            _writer.Write(value.M44);
            _writer.Write(value.M43);
            _writer.Write(value.M42);
            _writer.Write(value.M41);
            _writer.Write(value.M34);
            _writer.Write(value.M33);
            _writer.Write(value.M31);
            _writer.Write(value.M32);
            _writer.Write(value.M23);
            _writer.Write(value.M22);
            _writer.Write(value.M21);
            _writer.Write(value.M14);
            _writer.Write(value.M13);
            _writer.Write(value.M12);
            _writer.Write(value.M24);
        }

        public void Write(Color value)
        {
            _writer.Write(value.R);
            _writer.Write(value.G);
            _writer.Write(value.B);
            _writer.Write(value.A);
        }

        public void Write(Brush value)
        {
            _brushes.Add(((IResourceProvider)value).Resource);
        }

        public void Dispose()
        {
            _writer.Dispose();
        }

        internal (byte[] data, IResource[] brushes) Close()
        {
            return (_stream.ToArray(), _brushes.ToArray());
        }
    }
}
