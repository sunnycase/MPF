using MPF.Data;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public class DependencyObject
    {
        private readonly ConcurrentDictionary<DependencyProperty, object> _localValueStore = new ConcurrentDictionary<DependencyProperty, object>();
        private readonly ConcurrentDictionary<DependencyProperty, BindingBase> _bindings = new ConcurrentDictionary<DependencyProperty, BindingBase>();
        private readonly Type _realType;

        public DependencyObject()
        {
            _realType = this.GetType();
        }

        public void SetValue<T>(DependencyProperty<T> property, T value)
        {
            var oldValue = GetValue(property);
            if (!EqualityComparer<T>.Default.Equals(oldValue, value))
            {
                _localValueStore[property] = value;
                property.RaisePropertyChanged(_realType, this, new PropertyChangedEventArgs<T>(property, oldValue, value));
            }
        }

        public T GetValue<T>(DependencyProperty<T> property)
        {
            object oldValue;
            if (!_localValueStore.TryGetValue(property, out oldValue) &&
                !TryGetBindingValue(property, out oldValue))
            {
                T value;
                if (property.TryGetDefaultValue(_realType, out value))
                    return value;
                return default(T);
            }
            return (T)oldValue;
        }

        private bool TryGetBindingValue<T>(DependencyProperty<T> property, out object value)
        {
            BindingBase bindingBase;
            if (_bindings.TryGetValue(property, out bindingBase))
            {
                value = bindingBase.EffectiveValue.GetValue(null);
                return true;
            }
            value = null;
            return false;
        }

        internal void SetBinding(DependencyProperty property, BindingBase binding)
        {
            _bindings.AddOrUpdate(property, binding, (k, o) => binding);
        }

        internal void ClearBinding(DependencyProperty property)
        {
            BindingBase bindingBase;
            _bindings.TryRemove(property, out bindingBase);
        }

        internal BindingBase GetBinding(DependencyProperty property)
        {
            BindingBase bindingBase;
            _bindings.TryGetValue(property, out bindingBase);
            return bindingBase;
        }
    }

    public class PropertyChangedEventArgs<T> : EventArgs
    {
        public DependencyProperty<T> Property { get; }
        public T OldValue { get; }
        public T NewValue { get; }

        public PropertyChangedEventArgs(DependencyProperty<T> property, T oldValue, T newValue)
        {
            Property = property;
            OldValue = oldValue;
            NewValue = newValue;
        }
    }
}
