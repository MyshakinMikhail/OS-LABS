#pragma once

#include <windows.h>

void write_str(const char *s);

void read_line(char *buf, DWORD size);

int parse_two_floats(const char *buf, float *f1, float *f2);
