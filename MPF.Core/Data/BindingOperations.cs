using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public class BindingOperations
    {
        public static void SetBinding(DependencyObject d, DependencyProperty property, BindingBase binding)
        {
            //d.SetBinding(property, binding);
        }

        public static void ClearBinding(DependencyObject d, DependencyProperty property)
        {
            //d.ClearBinding(property);
        }

        public static BindingBase GetBinding(DependencyObject d, DependencyProperty property)
        {
            //return d.GetBinding(property);
            return null;
        }
    }
}
