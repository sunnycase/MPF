using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Data;

namespace MPF.Media
{
    public sealed class SolidColorBrush : Brush
    {
        private readonly Lazy<IResource> _brushResource;

        public static readonly DependencyProperty<Color> ColorProperty = DependencyProperty.Register(nameof(Color), typeof(SolidColorBrush),
            new PropertyMetadata<Color>(Colors.Transparent, OnColorPropertyChanged));

        public Color Color
        {
            get { return GetValue(ColorProperty); }
            set { this.SetLocalValue(ColorProperty, value); }
        }

        public SolidColorBrush()
        {
            _brushResource = this.CreateResource(ResourceType.RT_SolidColorBrush);
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            var color = Color.ToColorF();
            MediaResourceManager.Current.UpdateSolidColorBrush(_brushResource.Value, ref color);
        }

        internal override IResource GetResourceOverride() => _brushResource.Value;

        private static void OnColorPropertyChanged(object sender, PropertyChangedEventArgs<Color> e)
        {
            ((SolidColorBrush)sender).RegisterUpdateResource();
        }
    }
}
