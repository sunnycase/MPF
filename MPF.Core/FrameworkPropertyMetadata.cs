using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    [Flags]
    public enum FrameworkPropertyMetadataOptions
    {
        None = 0x0,
        Inherits = 0x1
    }

    public class FrameworkPropertyMetadata<T> : UIPropertyMetadata<T>
    {
        private readonly FrameworkPropertyMetadataOptions _fmkOptions;

        public FrameworkPropertyMetadata(T defaultValue, FrameworkPropertyMetadataOptions fmOptions = FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            :base(defaultValue, uiOptions, propertyChangedHandler)
        {
            _fmkOptions = fmOptions;
        }

        public FrameworkPropertyMetadata(DependencyProperty.UnsetValueType unsetValue, FrameworkPropertyMetadataOptions fmOptions = FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            :base(unsetValue, uiOptions, propertyChangedHandler)
        {
            _fmkOptions = fmOptions;
        }

        protected override bool TryGetDefaultValueOverride(DependencyObject d, DependencyProperty<T> property, out T value)
        {
            if(_fmkOptions.HasFlag(FrameworkPropertyMetadataOptions.Inherits))
            {
                var visual = d as Visual;
                if (visual != null)
                {
                    visual = VisualTreeHelper.GetParent(visual);
                    if (visual != null)
                    {
                        BindingOperations.SetBinding(d, property, new Binding
                        {
                            Path = property.Name,
                            Mode = BindingMode.OneWay,
                            Source = visual
                        });
                        value = visual.GetValue(property);
                        return true;
                    }
                }
            }
            return base.TryGetDefaultValueOverride(d, property, out value);
        }
    }
}
