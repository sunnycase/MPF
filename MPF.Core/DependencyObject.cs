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

        public DependencyValueStorageChain ValueStorage { get; } = new DependencyValueStorageChain();

        public readonly ConcurrentDictionary<DependencyProperty, Delegate> _propertyChangedHandlers = new ConcurrentDictionary<DependencyProperty, Delegate>();
        public readonly ConcurrentDictionary<DependencyProperty, Delegate> _propertyChangedHandlersGen = new ConcurrentDictionary<DependencyProperty, Delegate>();
        private Delegate _anyPropertyChangedHandler;

        public DependencyObject()
        {
            _realType = this.GetType();
            ValueStorage.CurrentValueChanged += valueStorage_CurrentValueChanged;
        }

        public T GetValue<T>(DependencyProperty<T> property)
        {
            T value;
            if (!(ValueStorage.TryGetCurrentValue(property, out value) ||
                property.TryGetNonDefaultValue(this, _realType, out value)))
                return GetDefaultValue(property);
            return value;
        }

        public void SetCurrentValue<T>(DependencyProperty<T> property, T value)
        {
            IEffectiveValue<T> eValue;
            if (ValueStorage.TryGetCurrentEffectiveValue(property, out eValue) && eValue.CanSetValue)
                eValue.Value = value;
            else
                this.SetLocalValue(property, value);
        }

        private readonly static MethodInfo _raisePropertyChangedHelper = typeof(DependencyObject).GetRuntimeMethods().Single(o => o.Name == nameof(RaisePropertyChangedHelper));
        private void valueStorage_CurrentValueChanged(object sender, CurrentValueChangedEventArgs e)
        {
            _raisePropertyChangedHelper.MakeGenericMethod(e.Property.PropertyType).Invoke(this, new object[] { e.Property, e });
        }

        public void RegisterPropertyChangedHandler<T>(DependencyProperty<T> property, EventHandler<PropertyChangedEventArgs<T>> handler)
        {
            _propertyChangedHandlers.AddOrUpdate(property, handler, (k, old) => Delegate.Combine(old, handler));
        }

        public void RemovePropertyChangedHandler<T>(DependencyProperty<T> property, EventHandler<PropertyChangedEventArgs<T>> handler)
        {
            Delegate d = null;
            _propertyChangedHandlers.TryRemove(property, out d);
            if (d != (Delegate)handler)
                _propertyChangedHandlers.AddOrUpdate(property, k => Delegate.Remove(d, handler), (k, old) => Delegate.Combine(old, Delegate.Remove(d, handler)));
        }

        public void RegisterPropertyChangedHandler(DependencyProperty property, EventHandler<PropertyChangedEventArgs> handler)
        {
            _propertyChangedHandlersGen.AddOrUpdate(property, handler, (k, old) => Delegate.Combine(old, handler));
        }

        public void RemovePropertyChangedHandler(DependencyProperty property, EventHandler<PropertyChangedEventArgs> handler)
        {
            Delegate d = null;
            _propertyChangedHandlersGen.TryRemove(property, out d);
            if (d != (Delegate)handler)
                _propertyChangedHandlersGen.AddOrUpdate(property, k => Delegate.Remove(d, handler), (k, old) => Delegate.Combine(old, Delegate.Remove(d, handler)));
        }

        public void RegisterAnyPropertyChangedHandler(EventHandler<PropertyChangedEventArgs> handler)
        {
            _anyPropertyChangedHandler = Delegate.Combine(_anyPropertyChangedHandler, handler);
        }

        public void RemoveAnyPropertyChangedHandler(EventHandler<PropertyChangedEventArgs> handler)
        {
            _anyPropertyChangedHandler = Delegate.Remove(_anyPropertyChangedHandler, handler);
        }

        internal void RaisePropertyChangedHelper<T>(DependencyProperty<T> property, CurrentValueChangedEventArgs e)
        {
            var oldValue = e.HasOldValue ? (T)e.OldValue : GetDefaultValue(property);
            var newValue = e.HasNewValue ? (T)e.NewValue : GetDefaultValue(property);

            if (e.HasOldValue && e.HasNewValue && EqualityComparer<T>.Default.Equals((T)e.OldValue, (T)e.NewValue))
                return;
            var args = new PropertyChangedEventArgs<T>(property, oldValue, newValue);
            property.RaisePropertyChanged(_realType, this, args);
            InvokeLocalPropertyChangedHandlers(args);
        }

        private void InvokeLocalPropertyChangedHandlers<T>(PropertyChangedEventArgs<T> e)
        {
            Delegate d;
            if (_propertyChangedHandlers.TryGetValue(e.Property, out d))
                ((EventHandler<PropertyChangedEventArgs<T>>)d)?.Invoke(this, e);

            if (_propertyChangedHandlersGen.TryGetValue(e.Property, out d))
                ((EventHandler<PropertyChangedEventArgs>)d)?.Invoke(this, e);
            ((EventHandler<PropertyChangedEventArgs>)_anyPropertyChangedHandler)?.Invoke(this, e);
        }

        private T GetDefaultValue<T>(DependencyProperty<T> property)
        {
            T value;
            if (property.TryGetDefaultValue(this, _realType, out value))
                return value;
            return default(T);
        }
    }

    public class PropertyChangedEventArgs : EventArgs
    {
        public DependencyProperty Property { get; }

        public PropertyChangedEventArgs(DependencyProperty property)
        {
            Property = property;
        }
    }

    public class PropertyChangedEventArgs<T> : PropertyChangedEventArgs
    {
        public new DependencyProperty<T> Property => (DependencyProperty<T>)base.Property;
        public T OldValue { get; }
        public T NewValue { get; }

        public PropertyChangedEventArgs(DependencyProperty<T> property, T oldValue, T newValue)
            : base(property)
        {
            OldValue = oldValue;
            NewValue = newValue;
        }
    }
}
