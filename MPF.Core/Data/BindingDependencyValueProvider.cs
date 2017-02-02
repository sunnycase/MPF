using MPF.Data;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq.Expressions;
using System.Reflection;
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

        internal class BindingEffectiveValue<T> : IEffectiveValue<T>
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
            private readonly List<Dependent> _dependents = new List<Dependent>();
            private T _oldValue;

            public BindingEffectiveValue(DependencyObject d, BindingBase binding)
            {
                _d = new WeakReference<DependencyObject>(d);
                Binding = binding ?? throw new ArgumentNullException(nameof(binding));

                var exp = Binding.GetExpression();
                _getter = exp.Compile();
                _oldValue = Value;
                GenerateDependents();
            }

            private void GenerateDependents()
            {
                foreach (var item in _dependents)
                    item.Changed -= Dependent_Changed;
                _dependents.Clear();

                DependencyObject d;
                if (_d.TryGetTarget(out d))
                {
                    var visitor = new Visitor(d, _dependents);
                    visitor.Visit(Binding.GetExpression());

                    foreach (var item in _dependents)
                        item.Changed += Dependent_Changed;
                }
            }

            private void Dependent_Changed(object sender, EventArgs e)
            {
                GenerateDependents();
                var newValue = Value;
                ValueChanged?.Invoke(this, new EffectiveValueChangedEventArgs(_oldValue, Value));
                _oldValue = newValue;
            }

            private T GetValue()
            {
                DependencyObject d;
                if (_d.TryGetTarget(out d))
                {
                    var value = _getter(d);
                    if (value is T) return (T)value;
                    else if (value != null)
                    {
                        var valueType = value.GetType();
                        var convert = TypeDescriptor.GetConverter(valueType);
                        if (convert.CanConvertTo(typeof(T)))
                            return (T)convert.ConvertTo(value, typeof(T));

                        convert = TypeDescriptor.GetConverter(typeof(T));
                        if (convert.CanConvertFrom(valueType))
                            return (T)convert.ConvertFrom(value);
                        throw new InvalidCastException($"Cannot cast from {value} to type {typeof(T)}");
                    }
                }
                return default(T);
            }

            private void SetValue(T value)
            {
                if (!CanSetValue)
                    throw new InvalidOperationException();
            }

            private class Visitor : ExpressionVisitor
            {
                public List<Dependent> Dependents { get; }
                private object _currentObj;

                public Visitor(object currentObj, List<Dependent> dependents)
                {
                    _currentObj = currentObj;
                    Dependents = dependents;
                }

                protected override Expression VisitMember(MemberExpression node)
                {
                    var member = node.Member;
                    if (member is PropertyInfo)
                    {
                        if (node.Expression.NodeType == ExpressionType.Constant)
                            _currentObj = ((ConstantExpression)node.Expression).Value;
                        else
                            throw new NotImplementedException();
                        Dependents.Add(new Dependent(_currentObj, node));
                    }
                    return base.VisitMember(node);
                }

                protected override Expression VisitMethodCall(MethodCallExpression node)
                {
                    if (node.Method.DeclaringType == typeof(DependencyObject) && node.Method.Name == "GetValue")
                    {
                        if (node.Object is ConstantExpression)
                            _currentObj = ((ConstantExpression)node.Object).Value;
                        Dependents.Add(new Dependent(_currentObj, node));
                    }
                    return base.VisitMethodCall(node);
                }
            }

            private class Dependent
            {
                private readonly DependencyProperty _dp;

                public event EventHandler Changed;

                public Dependent(object source, MemberExpression exp)
                {
                    bool registered = false;
                    if (source is DependencyObject)
                    {
                        _dp = DependencyProperty.FromName(exp.Member.Name, source.GetType());
                        if (_dp != null)
                        {
                            ((DependencyObject)source).RegisterPropertyChangedHandler(_dp, OnDependencyPropertyChanged);
                            registered = true;
                        }
                    }

                    if (!registered && source is INotifyPropertyChanged)
                    {

                    }
                }

                public Dependent(object source, MethodCallExpression exp)
                {
                    if (source is DependencyObject)
                    {
                        _dp = (DependencyProperty)((ConstantExpression)exp.Arguments[0]).Value;
                        ((DependencyObject)source).RegisterPropertyChangedHandler(_dp, OnDependencyPropertyChanged);
                    }
                }

                private void OnDependencyPropertyChanged(object sender, PropertyChangedEventArgs e)
                {
                    if (e.Property == _dp)
                        OnDependentChanged();
                }

                private void OnDependentChanged()
                {
                    Changed?.Invoke(this, EventArgs.Empty);
                }
            }
        }
    }
}

namespace MPF
{
    public static class BindingDependencyValueStyleProvider
    {
        public static Style SetBinding<T>(this Style style, DependencyProperty<T> property, BindingBase value)
        {
            style.SetValue(property, new BindingDependencyValueProvider<T>(value));
            return style;
        }

        class BindingDependencyValueProvider<T> : IEffectiveValueProvider<T>
        {
            private readonly BindingBase _binding;

            public BindingDependencyValueProvider(BindingBase binding)
            {
                _binding = binding;
            }

            public IEffectiveValue ProviderValue(DependencyObject d)
            {
                return new BindingDependencyValueProvider.BindingEffectiveValue<T>(d, _binding);
            }
        }
    }
}