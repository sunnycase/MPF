using System;
using System.Collections.Generic;
using System.Text;
using MPF.Interop;
using MPF.Internal.Media;
using MPF.Data;

namespace MPF.Media
{
    public sealed class ImageBrush : Brush
    {
        public static readonly DependencyProperty<ImageSource> SourceProperty = DependencyProperty.Register(nameof(Source), typeof(ImageBrush),
            new PropertyMetadata<ImageSource>(null, OnSourcePropertyChanged));

        public ImageSource Source
        {
            get => GetValue(SourceProperty);
            set => this.SetLocalValue(SourceProperty, value);
        }

        internal override IResource CreateSamplerResource()
        {
            return Sampler.Defalut.Resource;
        }

        internal override IResource CreateTextureResource()
        {
            return ((IResourceProvider)Source)?.Resource;
        }

        private static void OnSourcePropertyChanged(object sender, PropertyChangedEventArgs<ImageSource> e)
        {
            ((ImageBrush)sender).RegisterUpdateResource();
        }
    }
}
