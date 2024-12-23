#pragma once

typedef char* va_list;

#define va_start(args, last) ((args) = (char*)&(last) + 1)
#define va_arg(args, type) (*(type*)(args)++)
#define va_end(args)