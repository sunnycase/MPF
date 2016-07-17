using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Media;

namespace MPF.Controls
{
    public class Window : DependencyObject
    {
        private readonly CoreWindow _coreWindow;

        public static readonly DependencyProperty<bool> HasMaximizeProperty = DependencyProperty.Register(nameof(HasMaximize), typeof(Window), true, propertyChangedHandler: OnHasMaximizePropertyChanged);
        public static readonly DependencyProperty<string> TitleProperty = DependencyProperty.Register(nameof(Title), typeof(Window), string.Empty, propertyChangedHandler: OnTitlePropertyChanged);

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

        public Window()
        {
            _coreWindow = new CoreWindow();
            _coreWindow.HasMaximize = HasMaximize;
            _coreWindow.Title = Title;
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
