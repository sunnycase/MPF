using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Media;

namespace MPF.Controls
{
    public class Window : Decorator
    {
        private readonly CoreWindow _coreWindow = new CoreWindow();

        public static readonly DependencyProperty<bool> HasMaximizeProperty = DependencyProperty.Register(nameof(HasMaximize), typeof(Window),
            new PropertyMetadata<bool>(true, propertyChangedHandler: OnHasMaximizePropertyChanged));
        public static readonly DependencyProperty<string> TitleProperty = DependencyProperty.Register(nameof(Title), typeof(Window), 
            new PropertyMetadata<string>(string.Empty, propertyChangedHandler: OnTitlePropertyChanged));
        public static readonly DependencyProperty<Size> SizeProperty = DependencyProperty.Register(nameof(Size), typeof(Window),
            new UIPropertyMetadata<Size>(DependencyProperty.UnsetValue, UIPropertyMetadataOptions.AffectMeasure, OnSizePropertyChanged));

        public bool HasMaximize
        {
            get { return GetValue(HasMaximizeProperty); }
            set { SetValue(HasMaximizeProperty, value); }
        }

        public string Title
        {
            get { return GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        public Size Size
        {
            get { return GetValue(SizeProperty); }
            set { SetValue(SizeProperty, value); }
        }

        public Window()
        {
            _coreWindow.HasMaximize = HasMaximize;
            _coreWindow.Title = Title;
            Size = _coreWindow.Size;
            _coreWindow.SizeChanged += coreWindow_SizeChanged;

            _coreWindow.SetRootVisual(this);
        }

        private void coreWindow_SizeChanged(object sender, EventArgs e)
        {
            Size = _coreWindow.Size;
        }

        private static void OnSizePropertyChanged(object sender, PropertyChangedEventArgs<Size> e)
        {
            ((Window)sender)._coreWindow.Size = e.NewValue;
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            return _coreWindow.ClientSize;
        }

        public void Show()
        {
            _coreWindow.Show();
        }

        public void Hide()
        {
            _coreWindow.Hide();
        }

        private static void OnHasMaximizePropertyChanged(object sender, PropertyChangedEventArgs<bool> e)
        {
            ((Window)sender)._coreWindow.HasMaximize = e.NewValue;
        }

        private static void OnTitlePropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((Window)sender)._coreWindow.Title = e.NewValue;
        }
    }
}
