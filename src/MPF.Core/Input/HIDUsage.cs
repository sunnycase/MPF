using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Input
{
    [StructLayout(LayoutKind.Sequential)]
    public struct HIDUsage : IEquatable<HIDUsage>
    {
        public ushort Page;
        public ushort Id;

        public override bool Equals(object obj)
        {
            if (obj is HIDUsage)
                return Equals((HIDUsage)obj);
            return false;
        }

        public bool Equals(HIDUsage other)
        {
            return Page == other.Page && Id == other.Id;
        }

        public override int GetHashCode()
        {
            return Page.GetHashCode() ^ Id.GetHashCode();
        }

        public static bool operator==(HIDUsage left, HIDUsage right)
        {
            return left.Equals(right);
        }

        public static bool operator !=(HIDUsage left, HIDUsage right)
        {
            return !left.Equals(right);
        }

        public override string ToString()
        {
            return $"Page: {Page}, Id: {Id}";
        }
    }

    public static class HIDUsages
    {
        public static readonly HIDUsage[] None = new HIDUsage[0];

        public static readonly HIDUsage[] Default = new[]
        {
            GenericDesktop.Mouse, GenericDesktop.Keyboard
        };

        public static class GenericDesktop
        {
            private const ushort Page = 0x1;

            public static readonly HIDUsage Pointer = new HIDUsage { Page = Page, Id = 0x1 };
            public static readonly HIDUsage Mouse = new HIDUsage { Page = Page, Id = 0x2 };
            public static readonly HIDUsage Joystick = new HIDUsage { Page = Page, Id = 0x4 };
            public static readonly HIDUsage GamePad = new HIDUsage { Page = Page, Id = 0x5 };
            public static readonly HIDUsage Keyboard = new HIDUsage { Page = Page, Id = 0x6 };
        }
    }
}
