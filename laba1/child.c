#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const int BUFFER_SIZE = 256;

void FormatSum(float sum, char *out)
{
	int sign = 0;
	if (sum < 0.0f)
	{
		sign = 1;
		sum = -sum;
	}

	int intPart = (int)sum;
	int fracPart = (int)((sum - (float)intPart) * 100.0f + 0.5f);
	if (fracPart >= 100)
	{
		fracPart -= 100;
		intPart++;
	}

	char buf[64];
	if (sign)
	{
		snprintf(buf, sizeof(buf), "Сумма: -%d.%02d\n", intPart, fracPart);
	}
	else
	{
		snprintf(buf, sizeof(buf), "Сумма: %d.%02d\n", intPart, fracPart);
	}
	strcpy(out, buf);
}

int main()
{
	char line[BUFFER_SIZE];
	char c;
	int pos = 0;

	while (1)
	{
		ssize_t r = read(STDIN_FILENO, &c, 1);
		if (r <= 0)
			break;

		if (c == '\n')
		{
			line[pos] = '\0';
			if (pos == 0)
			{
				pos = 0;
				continue;
			}

			char *start = line;
			char *end;
			int count = 0;
			float nums[3];

			while (*start && count < 3)
			{
				while (*start == ' ' || *start == '\t')
					start++;
				if (!*start)
					break;

				nums[count] = strtof(start, &end);
				if (end == start)
					break;
				count++;
				start = end;
			}

			if (count == 3)
			{
				float sum = nums[0] + nums[1] + nums[2];
				char output[64];
				FormatSum(sum, output);
				write(STDOUT_FILENO, output, strlen(output));
			}
			pos = 0;
		}
		else
		{
			if (pos < BUFFER_SIZE - 1)
			{
				line[pos++] = c;
			}
		}
	}

	return 0;
}
