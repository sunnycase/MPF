//
// Tomato Media
// 不可复制基类
// 
// 作者：SunnyCase
// 创建日期 2015-08-16
#pragma once

struct NonCopyable
{
	NonCopyable() = default;
	NonCopyable(NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = default;
	NonCopyable& operator=(NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = default;
};