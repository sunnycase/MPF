using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public class BindingDependencyValueProvider : IDependencyValueProvider
    {
        public static BindingDependencyValueProvider Current { get; } = new BindingDependencyValueProvider();
        public float Priority => 2.0f;

        public void SetValue<T>(DependencyObject d, DependencyProperty<T> property, IDependencyValueStorage storage, BindingBase value)
        {
            storage.AddOrUpdate(this, property, o => new BindingEffectiveValue<T>(d, value), (k, o) => new BindingEffectiveValue<T>(d, value));
        }

        public bool TryGetValue<T>(DependencyProperty<T> property, IDependencyValueStorage storage, out BindingBase value)
        {
            IEffectiveValue<T> eValue;
            if (storage.TryGetValue(this, property, out eValue))
            {
                value = ((BindingEffectiveValue<T>)eValue).Binding;
                return true;
            }
            value = null;
            return false;
        }

        public void ClearValue<T>(DependencyProperty<T> property, IDependencyValueStorage storage)
        {
            IEffectiveValue<T> eValue;
            storage.TryRemove(this, property, out eValue);
        }

        class BindingEffectiveValue<T> : IEffectiveValue<T>
        {
            public EventHandler<EffectiveValueChangedEventArgs> ValueChanged { get; set; }

            public bool CanSetValue => Binding.Mode == BindingMode.TwoWay;
            
            public T Value
            {
                get => GetValue();
                set => SetValue(value);
            }

            public BindingBase Binding { get; }
            private readonly WeakReference<DependencyObject> _d;
            private readonly Func<DependencyObject, object> _getter;

            public BindingEffectiveValue(DependencyObject d, BindingBase binding)
            {
                _d = new WeakReference<DependencyObject>(d);
                Binding = binding ?? throw new ArgumentNullException(nameof(binding));
                _getter = Binding.GetExpression().Compile();
            }

            private T GetValue()
            {
                DependencyObject d;
                if (_d.TryGetTarget(out d))
                    return (T)_getter(d);
                return default(T);
            }

            private void SetValue(T value)
            {
                if (!CanSetValue)
                    throw new InvalidOperationException();
            }
        }
    }
}
