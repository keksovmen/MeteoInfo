#include "stdarg.h"



// Converts integer to string, returns length
extern "C" char* itoa(int value, char* str, int base = 10) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str + 1;
    }

    char* ptr = str;
    char* start = str;
    
    // Handle negative numbers
    if (value < 0 && base == 10) {
        *ptr++ = '-';
        start = ptr;
        value = -value;
    }
    
    // Convert digits (reverse order)
    char* rev_ptr = ptr;
    while (value > 0) {
        *rev_ptr++ = '0' + (value % base);
        value /= base;
    }
    
    // Terminate string
    *rev_ptr = '\0';
    
    // Reverse the string
    rev_ptr--;
    while (ptr < rev_ptr) {
        char tmp = *ptr;
        *ptr = *rev_ptr;
        *rev_ptr = tmp;
        ptr++;
        rev_ptr--;
    }
    
    return ptr + 1;
}

// Minimal sprintf that only handles %d
extern "C" int __wrap_sprintf(char* buffer, const char* fmt, ...) {
	va_list args;
    va_start(args, fmt);
    
    char* buf_ptr = buffer;
    const char* p = fmt;
    
    while (*p) {
        if (*p == '%' && *(p + 1) == 'd') {
            // Get integer argument
            int value = va_arg(args, int);
            
            // Convert integer to string
            buf_ptr = itoa(value, buf_ptr);
            p += 2;  // Skip "%d"
        } else {
            // Copy regular character
            *buf_ptr++ = *p++;
        }
    }
    
    // Null terminate
    *buf_ptr = '\0';
    
    return buf_ptr - buffer;
}

//hot to disable all delete something calls for _free_r
// void operator delete(void*){}