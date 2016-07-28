﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MPF.Interop;

namespace MPF.Media
{
    public sealed class SolidColorBrush : Brush
    {
        private readonly IResource _brushResource;

        public static readonly DependencyProperty<Color> ColorProperty = DependencyProperty.Register(nameof(Color), typeof(SolidColorBrush), Colors.Transparent,
            OnColorPropertyChanged);

        public Color Color
        {
            get { return GetValue(ColorProperty); }
            set { SetValue(ColorProperty, value); }
        }

        public SolidColorBrush()
        {
            _brushResource = MediaResourceManager.Current.CreateResouce(ResourceType.RT_SolidColorBrush);
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            var color = Color.ToColorF();
            MediaResourceManager.Current.UpdateSolidColorBrush(_brushResource, ref color);
        }

        internal override IResource GetResourceOverride() => _brushResource;

        private static void OnColorPropertyChanged(object sender, PropertyChangedEventArgs<Color> e)
        {
            ((SolidColorBrush)sender).RegisterUpdateResource();
        }
    }
}
