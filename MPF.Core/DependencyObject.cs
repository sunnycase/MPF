using MPF.Data;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Reflection;

namespace MPF
{
    public class DependencyObject
    {
        private readonly Type _realType;

        private readonly DependencyValueStorage _valueStorage = new DependencyValueStorage();
        public IDependencyValueStorage ValueStorage => _valueStorage;

        public DependencyObject()
        {
            _realType = this.GetType();
            _valueStorage.CurrentValueChanged += valueStorage_CurrentValueChanged;
        }

        public T GetValue<T>(DependencyProperty<T> property)
        {
            T value;
            if (!_valueStorage.TryGetCurrentValue(property, out value))
                return GetDefaultValue(property);
            return value;
        }

        public void SetCurrentValue<T>(DependencyProperty<T> property, T value)
        {
            IEffectiveValue<T> eValue;
            if (_valueStorage.TryGetCurrentEffectiveValue(property, out eValue) && eValue.CanSetValue)
                eValue.Value = value;
            else
                this.SetLocalValue(property, value);
        }

        private readonly static MethodInfo _raisePropertyChangedHelper = typeof(DependencyObject).GetRuntimeMethods().Single(o => o.Name == nameof(RaisePropertyChangedHelper));
        private void valueStorage_CurrentValueChanged(object sender, CurrentValueChangedEventArgs e)
        {
            _raisePropertyChangedHelper.MakeGenericMethod(e.Property.PropertyType).Invoke(this, new object[] { e.Property, e });
        }

        internal void RaisePropertyChangedHelper<T>(DependencyProperty<T> property, CurrentValueChangedEventArgs e)
        {
            var oldValue = e.HasOldValue ? (T)e.OldValue : GetDefaultValue(property);
            var newValue = e.HasNewValue ? (T)e.NewValue : GetDefaultValue(property);
            if (!EqualityComparer<T>.Default.Equals(oldValue, newValue))
            {
                property.RaisePropertyChanged(_realType, this, new PropertyChangedEventArgs<T>(property, oldValue, newValue));
            }
        }

        private T GetDefaultValue<T>(DependencyProperty<T> property)
        {
            T value;
            if (property.TryGetDefaultValue(this, _realType, out value))
                return value;
            return default(T);
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
