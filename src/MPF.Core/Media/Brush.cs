using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public abstract class Brush : Animatable, IResourceProvider
    {
        private readonly Lazy<IResource> _brushRes;
        IResource IResourceProvider.Resource => _brushRes.Value;

        internal Brush()
        {
            _brushRes = this.CreateResource(ResourceType.RT_Brush);
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdateBrush(_brushRes.Value, CreateTextureResource(), CreateSamplerResource());
        }

        internal abstract IResource CreateTextureResource();
        internal abstract IResource CreateSamplerResource();

        public static implicit operator Brush(Color color)
        {
            return (SolidColorBrush)(color);
        }
    }
}
