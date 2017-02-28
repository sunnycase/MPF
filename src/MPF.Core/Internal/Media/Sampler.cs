using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Internal.Media
{
    class Sampler : Animatable, IResourceProvider
    {
        private SamplerData _data;
        private readonly Lazy<IResource> _samplerRes;

        public Sampler(SamplerData data)
        {
            _data = data;
            _samplerRes = this.CreateResource(ResourceType.RT_Sampler);
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdateSampler(_samplerRes.Value, ref _data);
        }

        public static readonly Sampler Defalut = new Sampler(new SamplerData
        {
            AddressU = TextureAddress.TA_Wrap,
            AddressV = TextureAddress.TA_Wrap,
            AddressW = TextureAddress.TA_Wrap
        });

        public IResource Resource => _samplerRes.Value;
    }
}
