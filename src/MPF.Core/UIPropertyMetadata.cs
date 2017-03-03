using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    [Flags]
    public enum UIPropertyMetadataOptions
    {
        None = 0x0,
        AffectMeasure = 0x1,
        AffectArrange = 0x2,
        AffectRender = 0x4
    }

    public class UIPropertyMetadata<T> : PropertyMetadata<T>
    {
        private UIPropertyMetadataOptions _uiOptions;

        public UIPropertyMetadata(T defaultValue, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            :base(defaultValue, propertyChangedHandler)
        {
            _uiOptions = uiOptions;
        }

        public UIPropertyMetadata(DependencyProperty.UnsetValueType unsetValue, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            :base(unsetValue, propertyChangedHandler)
        {
            _uiOptions = uiOptions;
        }

        protected override void MergeOverride(PropertyMetadata<T> old)
        {
            base.MergeOverride(old);
            if (old is UIPropertyMetadata<T>)
                _uiOptions |= ((UIPropertyMetadata<T>)old)._uiOptions;
        }

        protected override void OnPropertyChanged(object sender, PropertyChangedEventArgs<T> e)
        {
            if (_uiOptions.HasFlag(UIPropertyMetadataOptions.AffectMeasure))
                (sender as UIElement)?.InvalidateMeasure();
            if (_uiOptions.HasFlag(UIPropertyMetadataOptions.AffectArrange))
                (sender as UIElement)?.InvalidateArrange();
            if (_uiOptions.HasFlag(UIPropertyMetadataOptions.AffectRender))
                (sender as UIElement)?.InvalidateRender();
            base.OnPropertyChanged(sender, e);
        }
    }
}
