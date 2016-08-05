using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    [Flags]
    public enum UIPropertyMetadataOptions
    {
        None = 0,
        AffectMeasure = 1
    }

    public class UIPropertyMetadata<T> : PropertyMetadata<T>
    {
        private readonly UIPropertyMetadataOptions _uiOptions;

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
            //var uiMetadata = (UIPropertyMetadata<T>)old;
        }

        protected override void OnPropertyChanged(object sender, PropertyChangedEventArgs<T> e)
        {
            if (_uiOptions.HasFlag(UIPropertyMetadataOptions.AffectMeasure))
                (sender as UIElement)?.RegisterMeasure();
            base.OnPropertyChanged(sender, e);
        }
    }
}
