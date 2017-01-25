using System;
using System.Collections.Generic;
using System.Linq;
using System.Linq.Expressions;
using System.Reflection;
using System.Text;

namespace MPF.Data
{
    public class PropertyPath
    {
        public string Path { get; }
        public DependencyProperty[] Properties { get; }

        public PropertyPath(string path, params DependencyProperty[] properties)
        {
            Path = path;
            Properties = properties;
        }

        public Expression GetAccessor(Expression source)
        {
            var result = source;
            if (!string.IsNullOrEmpty(Path))
            {
                var dps = new Stack<DependencyPropertyPos>();
                var property = new PropertyPos();
                var dpp = new DependencyPropertyPos();

                int index = 0;
                if (Path[0] != '{') property.Start = 0;
                foreach (var c in Path)
                {
                    if (c == '{')
                    {
                        dpp.BracketStart = index;
                    }
                    else if (c == '}')
                    {
                        dpp.BracketEnd = index;
                    }
                    else if (c == '.')
                    {
                        if (dpp.BracketEnd != null)
                        {
                            result = BuildExpression(result, ref dpp);
                        }
                        else
                        {
                            if (property.Start == null)
                                property.Start = index + 1;
                            else
                            {
                                property.End = index - 1;

                                result = BuildExpression(result, ref property);
                            }
                        }
                    }
                    index++;
                }

                if (property.Start != null)
                {
                    property.End = index - 1;
                    result = BuildExpression(result, ref property);
                }
                else if (dpp.BracketEnd != null)
                    result = BuildExpression(result, ref dpp);
            }
            return Expression.Convert(result, typeof(object));
        }

        private Expression BuildExpression(Expression result, ref PropertyPos property)
        {
            result = Expression.Property(result, Path.Substring(property.Start.Value, (property.End - property.Start).Value + 1));
            property.End = property.Start = null;
            return result;
        }

        private static readonly MethodInfo _getValueMethodGen = typeof(DependencyObject).GetTypeInfo().DeclaredMethods
            .Single(o => o.Name == nameof(DependencyObject.GetValue));

        private Expression BuildExpression(Expression result, ref DependencyPropertyPos dpp)
        {
            var dp = Properties[int.Parse(Path.Substring(dpp.BracketStart.Value + 1, (dpp.BracketEnd - dpp.BracketStart).Value - 1))];
            var getValueMethod = _getValueMethodGen.MakeGenericMethod(dp.PropertyType);
            result = Expression.Call(result, getValueMethod, Expression.Constant(dp));
            dpp.BracketStart = dpp.BracketEnd = null;
            return result;
        }

        struct PropertyPos
        {
            public int? Start { get; set; }
            public int? End { get; set; }
        }

        struct DependencyPropertyPos
        {
            public int? BracketStart { get; set; }
            public int? BracketEnd { get; set; }
        }

        public static implicit operator PropertyPath(string path)
        {
            return new PropertyPath(path);
        }
    }
}
