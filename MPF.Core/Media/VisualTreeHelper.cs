using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media
{
    public static class VisualTreeHelper
    {
        public static Visual GetParent(Visual visual)
        {
            return visual.Parent;
        }

        public static void HitTest(Visual visual, PointHitTestParameters param, HitTestFilterCallback<Visual> filter, HitTestResultCallback<PointHitTestResult> resultCallback)
        {
            visual.HitTest(param, filter, resultCallback);
        }
    }
}
