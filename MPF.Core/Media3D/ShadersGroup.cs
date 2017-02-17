using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;
using MPF.Interop;

namespace MPF.Media3D
{
    public abstract class ShadersGroup : Animatable, IResourceProvider
    {
        private readonly Lazy<IResource> _shadersRes;

        internal ShadersGroup()
        {
            _shadersRes = this.CreateResource(ResourceType.RT_ShadersGroup);
            RegisterUpdateResource();
        }

        internal unsafe override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);

            (var vs, var ps) = GetShaderBytesCode();
            fixed (byte* vsPtr = vs)
            fixed (byte* psPtr = ps)
            {
                var data = new ShadersGroupData
                {
                    VertexShader = new IntPtr(vsPtr),
                    VertexShaderLength = (uint?)vs?.Length ?? 0,
                    PixelShader = new IntPtr(psPtr),
                    PixelShaderLength = (uint?)ps?.Length ?? 0
                };
                MediaResourceManager.Current.UpdateShadersGroup(_shadersRes.Value, ref data);
            }
        }

        protected abstract (byte[] vertexShader, byte[] pixelShader) GetShaderBytesCode();

        private static readonly Lazy<ShadersGroup> _standard = new Lazy<ShadersGroup>(OnCreateStandard);
        public static ShadersGroup Standard => _standard.Value;

        IResource IResourceProvider.Resource => _shadersRes.Value;

        private static ShadersGroup OnCreateStandard()
        {
            return null;
        }
    }

    public abstract class ShadersGroup<T> : ShadersGroup where T : ShaderParameters
    {
        public ShadersGroup()
        {

        }
    }
}
