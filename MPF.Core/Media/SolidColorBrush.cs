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

        private ColorF _color;

        public SolidColorBrush()
        {
            _brushResource = this.CreateResource(ResourceType.RT_SolidColorBrush);
            OnColorChanged(Color.ToColorF());
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, EventArgs.Empty);
            MediaResourceManager.Current.UpdateSolidColorBrush(_brushResource.Value, ref _color);
        }

        private void OnColorChanged(ColorF color)
        {
            _color = color;
            RegisterUpdateResource();
        }

        internal override IResource GetResourceOverride() => _brushResource.Value;

        private static void OnColorPropertyChanged(object sender, PropertyChangedEventArgs<Color> e)
        {
            var brush = (SolidColorBrush)sender;
            brush.OnColorChanged(e.NewValue.ToColorF());
        }

        public static implicit operator SolidColorBrush(Color color)
        {
            return new SolidColorBrush { Color = color };
        }
    }
}
