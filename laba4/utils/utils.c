#include <stdlib.h>
#include <windows.h>
#include <string.h>

void write_str(const char *s)
{
	DWORD written;
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(hOut, s, (DWORD)strlen(s), &written, NULL);
}

void read_line(char *buf, DWORD size)
{
	DWORD read;
	HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
	ReadConsoleA(hIn, buf, size - 1, &read, NULL);
	for (DWORD i = 0; i < read; i++)
	{
		if (buf[i] == '\r' || buf[i] == '\n')
		{
			buf[i] = '\0';
			break;
		}
	}
}

int parse_two_floats(const char *buf, float *f1, float *f2)
{
	char *end;
	*f1 = strtof(buf, &end);
	if (end == buf)
		return 0;
	*f2 = strtof(end, &end);
	if (end == buf)
		return 0;
	return 1;
}