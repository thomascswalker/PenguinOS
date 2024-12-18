#pragma once

typedef int* va_list;

#define va_start(args, last) ((args) = (int*)&(last) + 1)
#define va_arg(args, type) (*(type*)(args)++)
#define va_end(args)