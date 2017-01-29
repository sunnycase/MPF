using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Media;
using MPF.Data;

namespace MPF.Controls
{
    public class Window : ContentControl
    {
        private readonly CoreWindow _coreWindow = new CoreWindow();

        public static readonly DependencyProperty<bool> HasMaximizeProperty = DependencyProperty.Register(nameof(HasMaximize), typeof(Window),
            new PropertyMetadata<bool>(true, propertyChangedHandler: OnHasMaximizePropertyChanged));
        public static readonly DependencyProperty<string> TitleProperty = DependencyProperty.Register(nameof(Title), typeof(Window),
            new PropertyMetadata<string>(string.Empty, propertyChangedHandler: OnTitlePropertyChanged));

        public bool HasMaximize
        {
            get { return GetValue(HasMaximizeProperty); }
            set { this.SetLocalValue(HasMaximizeProperty, value); }
        }

        public string Title
        {
            get { return GetValue(TitleProperty); }
            set { this.SetLocalValue(TitleProperty, value); }
        }

        public Window()
        {
            _coreWindow.HasMaximize = HasMaximize;
            _coreWindow.Title = Title;
            _coreWindow.Size = new Size(Width, Height);

            _coreWindow.SetRootVisual(this);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            _coreWindow.Size = finalSize;
            return base.ArrangeOverride(_coreWindow.ClientSize);
        }

        public void Show()
        {
            _coreWindow.Show();
            InvalidateArrange();
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
