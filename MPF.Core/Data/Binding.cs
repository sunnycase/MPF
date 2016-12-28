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

        private static readonly MethodInfo _getServiceMethod = typeof(IServiceProvider).GetTypeInfo().GetDeclaredMethod(nameof(IServiceProvider.GetService));

        protected override Expression<Func<IServiceProvider, object>> CreateExpression()
        {
            var serviceProvider = Expression.Parameter(typeof(object), "s");
            var source = _hasSource ? (Expression)Expression.Constant(Source) :
                Expression.Call(serviceProvider, _getServiceMethod, Expression.Constant(null, typeof(Type)));
            var body = Path == null ? source : Path.GetAccessor(source);
            return (Expression<Func<IServiceProvider, object>>)Expression.Lambda<Func<IServiceProvider, object>>(body, serviceProvider).Reduce();
        }
    }
}
