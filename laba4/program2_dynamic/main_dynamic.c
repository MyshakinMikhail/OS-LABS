#include <windows.h>
#include <stdio.h>
#include "contract.h"
#include "./../utils/utils.h"

#define BUF_SIZE 256

int main()
{
	HMODULE lib1 = LoadLibraryA("libimpl1.dll");
	HMODULE lib2 = LoadLibraryA("libimpl2.dll");
	if (!lib1 || !lib2)
		return 1;

	HMODULE currentLib = lib1;

	cos_derivative_func cos_derivative = (cos_derivative_func)GetProcAddress(currentLib, "cos_derivative");
	area_func area = (area_func)GetProcAddress(currentLib, "area");
	if (!cos_derivative || !area)
		return 2;

	char buf[BUF_SIZE];
	char out[BUF_SIZE];

	write_str("Loaded: libimpl1.dll\n");
	write_str("Commands:\n0 -> switch implementation\n1 a dx -> cos_derivative(a, dx)\n2 a b -> area(a, b)\n");

	while (1)
	{
		read_line(buf, BUF_SIZE);

		if (buf[0] == '0')
		{
			currentLib = (currentLib == lib1) ? lib2 : lib1;
			cos_derivative = (cos_derivative_func)GetProcAddress(currentLib, "cos_derivative");
			area = (area_func)GetProcAddress(currentLib, "area");
			snprintf(out, BUF_SIZE, "Loaded: %s\n", currentLib == lib1 ? "libimpl1.dll" : "libimpl2.dll");
			write_str(out);
		}
		else if (buf[0] == '1')
		{
			float a, dx;
			if (!parse_two_floats(buf + 2, &a, &dx))
			{
				write_str("Invalid input\n");
				continue;
			}
			float res = cos_derivative(a, dx);
			snprintf(out, BUF_SIZE, "Result = %f\n", res);
			write_str(out);
		}
		else if (buf[0] == '2')
		{
			float a, b;
			if (!parse_two_floats(buf + 2, &a, &b))
			{
				write_str("Invalid input\n");
				continue;
			}
			float res = area(a, b);
			snprintf(out, BUF_SIZE, "Result = %f\n", res);
			write_str(out);
		}
		else
		{
			write_str("Unknown command\n");
		}
	}

	FreeLibrary(lib1);
	FreeLibrary(lib2);
	return 0;
}
