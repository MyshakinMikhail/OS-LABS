#include <stdio.h>
#include "contract.h"

int main()
{
	int cmd;

	printf("Commands:\n"
		   "1 a dx -> cos_derivative(a, dx)\n"
		   "2 a b -> area(a, b)\n"
		   "0 -> exit\n");

	while (scanf("%d", &cmd) == 1)
	{
		if (cmd == 0)
			break;

		if (cmd == 1)
		{
			float a, dx;
			scanf("%f %f", &a, &dx);
			printf("Result = %f\n", cos_derivative(a, dx));
		}
		else if (cmd == 2)
		{
			float a, b;
			scanf("%f %f", &a, &b);
			printf("Result = %f\n", area(a, b));
		}
	}
}