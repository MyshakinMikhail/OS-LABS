#pragma once


#ifdef _WIN32
#define API __declspec(dllexport)
#endif


API float cos_derivative(float a, float dx);
API float area(float a, float b);