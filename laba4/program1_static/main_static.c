#include "contract.h"
#include <stdio.h>
#include "./../utils/utils.h"

#define BUF_SIZE 256

extern float cos_derivative(float a, float dx);
extern float area(float a, float b);

int main()
{
	char buf[BUF_SIZE];
	char out[BUF_SIZE];

	write_str("Program 1 (static linking)\n");
	write_str("Commands:\n0 -> exit\n1 a dx -> cos_derivative(a, dx)\n2 a b -> area(a, b)\n");

	while (1)
	{
		read_line(buf, BUF_SIZE);

		if (buf[0] == '0')
		{
			break;
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

	return 0;
}
