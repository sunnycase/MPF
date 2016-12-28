using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Text;

namespace MPF.Data
{
    public abstract class BindingBase
    {
        private bool _isSealed = false;
        private EffectiveValue _effectiveValue;

        internal EffectiveValue EffectiveValue
        {
            get
            {
                Seal();
                return _effectiveValue;
            }
        }

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
                _effectiveValue = new BindingEffectiveValue(CreateExpression());
                _isSealed = true;
            }
        }

        protected void CheckSealed()
        {
            if (_isSealed)
                throw new InvalidOperationException("Binding is already sealed.");
        }

        private class BindingEffectiveValue : EffectiveValue
        {
            private readonly Expression<Func<IServiceProvider, object>> _expression;

            public BindingEffectiveValue(Expression<Func<IServiceProvider, object>> expression)
            {
                _expression = expression;
            }

            public override object GetValue(IServiceProvider serviceProvider)
            {
                return _expression.Compile()(serviceProvider);
            }
        }
    }
}
