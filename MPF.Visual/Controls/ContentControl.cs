using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ContentControl : Control
    {
        public ContentControl()
        {
            Template = ControlTemplate.Create<ContentControl>(t =>
                new TextBlock
                {
                    Margin = new Thickness(5),
                    FontSize = 15,
                    FontFamily = new FontFamily("Microsoft YaHei"),
                    Text = "baka帝球"
                });
        }
    }
}
