using MPF.Controls.Primitives;
using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class Button : ButtonBase
    {
        static Button()
        {
            StyleProperty.OverrideMetadata(typeof(Button), new PropertyMetadata<Style>(
                new Style(typeof(Button))
                    .SetLocalValue(Button.TemplateProperty, ControlTemplate.Create<Button>(t=>
                    {
                        var root = new Border();
                        BindingOperations.SetBinding(root, Border.BorderThicknessProperty, new Binding { Path = new PropertyPath("BorderThickness"), Source = t });
                        BindingOperations.SetBinding(root, Border.BackgroundProperty, new Binding { Path = new PropertyPath("Background"), Source = t });
                        BindingOperations.SetBinding(root, Border.BorderBrushProperty, new Binding { Path = new PropertyPath("BorderBrush"), Source = t });

                        return root;
                    }))));
        }
    }
}
