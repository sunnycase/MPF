using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public class PropertyMetadata<T>
    {
        private bool _defaultValueSet;
        private T _defaultValue;

        public event EventHandler<PropertyChangedEventArgs<T>> PropertyChanged;

        public PropertyMetadata(T defaultValue, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
        {
            _defaultValue = defaultValue;
            _defaultValueSet = true;
            if (propertyChangedHandler != null)
                PropertyChanged += propertyChangedHandler;
        }

        public PropertyMetadata(DependencyProperty.UnsetValueType unsetValue, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
        {
            _defaultValueSet = false;
            if (propertyChangedHandler != null)
                PropertyChanged += propertyChangedHandler;
        }

        public bool TryGetDefaultValue(out T value)
        {
            if (_defaultValueSet)
            {
                value = _defaultValue;
                return true;
            }
            value = default(T);
            return false;
        }

        internal void RaisePropertyChanged(object sender, PropertyChangedEventArgs<T> e)
        {
            PropertyChanged?.Invoke(sender, e);
        }

        protected virtual void OnPropertyChanged(object sender, PropertyChangedEventArgs<T> e)
        {

        }

        protected virtual void MergeOverride(PropertyMetadata<T> old)
        {
            if (!_defaultValueSet && old._defaultValueSet)
            {
                _defaultValue = old._defaultValue;
                _defaultValueSet = true;
            }
            PropertyChanged = (EventHandler<PropertyChangedEventArgs<T>>)Delegate.Combine(old.PropertyChanged, PropertyChanged);
        }

        internal void Merge(PropertyMetadata<T> old)
        {
            MergeOverride(old);
        }
    }
}
