using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;
using System.Linq;

namespace MPF.Data
{
    public class Binding : BindingBase
    {
        private bool _hasSource = false;
        private object _source;
        public object Source
        {
            get { return _source; }
            set
            {
                CheckSealed();
                _source = value;
                _hasSource = true;
            }
        }

        private PropertyPath _path;
        public PropertyPath Path
        {
            get { return _path; }
            set
            {
                CheckSealed();
                _path = value;
            }
        }

        protected override Expression<Func<object, object>> CreateExpression()
        {
            var dataContext = Expression.Parameter(typeof(object), "s");
            var source = _hasSource ? (Expression)Expression.Constant(Source) : dataContext;
            var body = Path == null ? source : Path.GetAccessor(source);
            return (Expression<Func<object, object>>)Expression.Lambda<Func<object, object>>(body, dataContext).Reduce();
        }
    }
}
