#include "../base/microsleep.h"

// unicode type
void unicodeType(const unsigned value)
{
#if defined(IS_MACOSX)
	UniChar ch = (UniChar)value; // Convert to unsigned char

	toggleUnicode(ch, true);
	toggleUnicode(ch, false);
#elif defined(IS_WINDOWS)
	INPUT input[2];
	memset(input, 0, sizeof(input));

	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = value;
	input[0].ki.dwFlags = 0x4; // KEYEVENTF_UNICODE;

	input[1].type = INPUT_KEYBOARD;
	input[1].ki.wVk = 0;
	input[1].ki.wScan = value;
	input[1].ki.dwFlags = KEYEVENTF_KEYUP | 0x4; // KEYEVENTF_UNICODE;

	SendInput(2, input, sizeof(INPUT));
#elif defined(USE_X11)
	toggleUniKey(value, true);
	toggleUniKey(value, false);
#endif
}

void typeStringDelayed(const char *str, const unsigned cpm)
{

	/* Characters per second */
	const double cps = (double)cpm / 60.0;

	/* Average milli-seconds per character */
	const double mspc = (cps == 0.0) ? 0.0 : 1000.0 / cps;

	unsigned long n;
	unsigned short c;
	unsigned short c1;
	unsigned short c2;
	unsigned short c3;

	while (*str != '\0')
	{
		c = *str++;

		// warning, the following utf8 decoder
		// doesn't perform validation
		if (c <= 0x7F)
		{
			// 0xxxxxxx one byte
			n = c;
		}
		else if ((c & 0xE0) == 0xC0)
		{
			// 110xxxxx two bytes
			c1 = (*str++) & 0x3F;
			n = ((c & 0x1F) << 6) | c1;
		}
		else if ((c & 0xF0) == 0xE0)
		{
			// 1110xxxx three bytes
			c1 = (*str++) & 0x3F;
			c2 = (*str++) & 0x3F;
			n = ((c & 0x0F) << 12) | (c1 << 6) | c2;
		}
		else if ((c & 0xF8) == 0xF0)
		{
			// 11110xxx four bytes
			c1 = (*str++) & 0x3F;
			c2 = (*str++) & 0x3F;
			c3 = (*str++) & 0x3F;
			n = ((c & 0x07) << 18) | (c1 << 12) | (c2 << 6) | c3;
		}

		unicodeType(n);

		if (mspc > 0)
		{
			microsleep(mspc + (DEADBEEF_UNIFORM(0.0, 0.5)));
		}
	}
}