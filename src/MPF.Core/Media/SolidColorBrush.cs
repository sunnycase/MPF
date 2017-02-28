using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Data;
using System.Collections.Concurrent;
using MPF.Internal.Media;

namespace MPF.Media
{
    public sealed class SolidColorBrush : Brush
    {
        public static readonly DependencyProperty<Color> ColorProperty = DependencyProperty.Register(nameof(Color), typeof(SolidColorBrush),
            new PropertyMetadata<Color>(Colors.Transparent, OnColorPropertyChanged));

        public Color Color
        {
            get { return GetValue(ColorProperty); }
            set { this.SetLocalValue(ColorProperty, value); }
        }

        private ColorF _color;

        public SolidColorBrush()
        {

        }

        private void OnColorChanged(ColorF color)
        {
            _color = color;
            RegisterUpdateResource();
        }

        internal override IResource CreateTextureResource()
        {
            return SolidColorTexture.GetOrAdd(Color).Resource;
        }

        internal override IResource CreateSamplerResource()
        {
            return Sampler.Defalut.Resource;
        }

        private static void OnColorPropertyChanged(object sender, PropertyChangedEventArgs<Color> e)
        {
            var brush = (SolidColorBrush)sender;
            brush.OnColorChanged(e.NewValue.ToColorF());
        }

        public static implicit operator SolidColorBrush(Color color)
        {
            return new SolidColorBrush { Color = color };
        }

        class SolidColorTexture : Animatable, IResourceProvider
        {
            private readonly Lazy<IResource> _textureRes;
            private ColorF _color;

            public SolidColorTexture(Color color)
            {
                _color = color.ToColorF();
                _textureRes = this.CreateResource(ResourceType.RT_SolidColorTexture);
                RegisterUpdateResource();
            }

            internal override void OnUpdateResource(object sender, EventArgs e)
            {
                base.OnUpdateResource(sender, e);
                MediaResourceManager.Current.UpdateSolidColorTexture(_textureRes.Value, ref _color);
            }

            public IResource Resource => _textureRes.Value;

            private static readonly ConcurrentDictionary<Color, WeakReference<SolidColorTexture>> _solidColorTextures = new ConcurrentDictionary<Color, WeakReference<SolidColorTexture>>();

            public static SolidColorTexture GetOrAdd(Color color)
            {
                var weak = _solidColorTextures.GetOrAdd(color, k => new WeakReference<SolidColorTexture>(new SolidColorTexture(k)));
                SolidColorTexture result;
                if (!weak.TryGetTarget(out result))
                {
                    result = new SolidColorTexture(color);
                    weak.SetTarget(result);
                }
                return result;
            }
        }
    }
}
