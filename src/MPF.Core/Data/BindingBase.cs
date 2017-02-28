using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Text;

namespace MPF.Data
{
    public abstract class BindingBase
    {
        private bool _isSealed = false;
        private Expression<Func<object, object>> _expression;

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

        protected abstract Expression<Func<object, object>> CreateExpression();
        public Expression<Func<object, object>> GetExpression()
        {
            Seal();
            return _expression;
        }

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
    }
}
