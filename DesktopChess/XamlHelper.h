
#include <stdarg.h>
#include <strsafe.h>

Platform::String^ MakeString(wchar_t* format, ...)
{
	va_list vl;
	va_start(vl, format);

	wchar_t buffer[100];

	::StringCchVPrintf(buffer, 100, format, vl);

	return ref new Platform::String(buffer);
}

Windows::Foundation::TimeSpan MakeTimeSpan(int milliseconds)
{
	auto ts = Windows::Foundation::TimeSpan();
	ts.Duration = milliseconds * 10 * 1000;
	return ts;
}