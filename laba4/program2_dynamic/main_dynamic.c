#include <stdio.h>
#include <windows.h>

typedef float (*fn_derivative)(float, float);
typedef float (*fn_area)(float, float);

HMODULE lib = NULL;
fn_derivative fder = NULL;
fn_area farea = NULL;

void load_library(const char *name)
{
	if (lib)
		FreeLibrary(lib);

	lib = LoadLibraryA(name);

	if (!lib)
	{
		printf("Error loading %s\n", name);
		exit(1);
	}

	fder = (fn_derivative)GetProcAddress(lib, "cos_derivative");
	farea = (fn_area)GetProcAddress(lib, "area");

	if (!fder || !farea)
	{
		printf("Invalid library: missing contracts\n");
		exit(1);
	}

	printf("Loaded: %s\n", name);
}

int main()
{
	int state = 0;
	load_library("libimpl1.dll");

	printf("Commands:\n"
		   "0 -> switch implementation\n"
		   "1 a dx -> cos_derivative(a, dx)\n"
		   "2 a b -> area(a, b)\n");

	int cmd;
	while (scanf("%d", &cmd) == 1)
	{
		if (cmd == 0)
		{
			state ^= 1;
			load_library(state ? "libimpl2.dll" : "libimpl1.dll");
		}
		else if (cmd == 1)
		{
			float a, dx;
			scanf("%f %f", &a, &dx);
			printf("Result = %f\n", fder(a, dx));
		}
		else if (cmd == 2)
		{
			float a, b;
			scanf("%f %f", &a, &b);
			printf("Result = %f\n", farea(a, b));
		}
	}
}