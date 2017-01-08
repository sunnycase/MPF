using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Text;

namespace MPF.Data
{
    public abstract class BindingBase
    {
        private bool _isSealed = false;
        private Expression<Func<IServiceProvider, object>> _expression;

        private BindingMode _mode = BindingMode.OneTime;
        public BindingMode Mode
        {
            get { return _mode; }
            set
            {
                CheckSealed();
                _mode = value;
            }
        }

        protected abstract Expression<Func<IServiceProvider, object>> CreateExpression();

        private void Seal()
        {
            if (!_isSealed)
            {
                _expression = CreateExpression();
                _isSealed = true;
            }
        }

        protected void CheckSealed()
        {
            if (_isSealed)
                throw new InvalidOperationException("Binding is already sealed.");
        }

        internal BindingEffectiveValue<T> CreateEffectiveValue<T>(IServiceProvider serviceProvider)
        {
            Seal();
            return new BindingEffectiveValue<T>(CreateExpression(), serviceProvider);
        }
    }

    internal class BindingEffectiveValue<T> : IEffectiveValue<T>
    {
        private readonly Expression<Func<IServiceProvider, object>> _expression;
        private readonly IServiceProvider _serviceProvider;

        public BindingEffectiveValue(Expression<Func<IServiceProvider, object>> expression, IServiceProvider serviceProvider)
        {
            _expression = expression;
            _serviceProvider = serviceProvider;
        }

        public T Value => (T)_expression.Compile()(_serviceProvider);

        public EventHandler ValueChanged { get; set; }

    }
}
