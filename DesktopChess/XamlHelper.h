
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