using MPF.Controls.Primitives;
using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;
using MPF.Input;
using System.Diagnostics;

namespace MPF.Controls
{
    public class Button : ButtonBase
    {
        static Button()
        {
            StyleProperty.OverrideMetadata(typeof(Button), new PropertyMetadata<Style>(
                new Style(typeof(Button))
                    .SetLocalValue(Button.TemplateProperty, ControlTemplate.Create<Button>(t =>
                    {
                        var cp = new ContentPresenter();
                        BindingOperations.SetBinding(cp, ContentPresenter.ContentProperty, new Binding { Path = "Content", Source = t, Mode = BindingMode.OneWay });
                        BindingOperations.SetBinding(cp, ContentPresenter.ContentTemplateProperty, new Binding { Path = "ContentTemplate", Source = t, Mode = BindingMode.OneWay });
                        BindingOperations.SetBinding(cp, ContentPresenter.HorizontalAlignmentProperty, new Binding { Path = "HorizontalContentAlignment", Source = t, Mode = BindingMode.OneWay });
                        BindingOperations.SetBinding(cp, ContentPresenter.VerticalAlignmentProperty, new Binding { Path = "VerticalContentAlignment", Source = t, Mode = BindingMode.OneWay });

                        var root = new Border
                        {
                            Child = cp
                        };
                        BindingOperations.SetBinding(root, Border.BorderThicknessProperty, new Binding { Path = new PropertyPath("BorderThickness"), Source = t });
                        BindingOperations.SetBinding(root, Border.BackgroundProperty, new Binding { Path = new PropertyPath("Background"), Source = t });
                        BindingOperations.SetBinding(root, Border.BorderBrushProperty, new Binding { Path = new PropertyPath("BorderBrush"), Source = t });

                        return root;
                    }))
                    .SetLocalValue(Button.HorizontalContentAlignmentProperty, HorizontalAlignment.Center)
                    .SetLocalValue(Button.VerticalContentAlignmentProperty, VerticalAlignment.Center)
                    ));
        }
    }
}
