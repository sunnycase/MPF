using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;
using MPF.Interop;

namespace MPF.Media3D
{
    public abstract class ShadersGroup : Freezable, IResourceProvider
    {
        private readonly Lazy<IResource> _shadersRes;

        internal ShadersGroup()
        {
            _shadersRes = this.CreateResource(ResourceType.RT_BoxGeometry3D);
        }

        protected abstract void OnCreateShaders();

        protected void CreateShaders(IntPtr vertexShader, IntPtr pixelShader)
        {

        }

        private static readonly Lazy<ShadersGroup> _standard = new Lazy<ShadersGroup>(OnCreateStandard);
        public static ShadersGroup Standard => _standard.Value;

        IResource IResourceProvider.Resource => OnGetShaderResource();

        private IResource OnGetShaderResource()
        {
            if(!IsFrozen)
            {
                OnCreateShaders();
                Freeze();
            }
            return _shadersRes.Value;
        }

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
