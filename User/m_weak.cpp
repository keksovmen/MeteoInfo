#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>



extern "C"
{
	//declaration from libnano used to write to uart as debug
	extern int _write(int file, const char *ptr, int len);
}

static void _putchar(char c)
{
	_write(1, &c, 1);
}

static void _putInt(int i)
{
	char buff[16] = {0};
	const char* end = itoa(i, buff, 10);
	_write(1, buff, end - buff);
}



// Converts integer to string, returns length
extern "C" char* itoa(int value, char* str, int base = 10) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return str + 1;
    }

    char* ptr = str;
    
    // Handle negative numbers
    if (value < 0 && base == 10) {
        *ptr++ = '-';
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
	char* end = rev_ptr;
    
    // Reverse the string
    rev_ptr--;
    while (ptr < rev_ptr) {
        char tmp = *ptr;
        *ptr = *rev_ptr;
        *rev_ptr = tmp;
        ptr++;
        rev_ptr--;
    }
    
    return end;
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



//how to disable all delete something calls for _free_r
//it defines global destructor for C++, all calls to delete will be redirected here
//since we do not use them, we should be fine
void operator delete(void*){}




//this will decrease printf size and override lib implementation, that uses bunch of other functions and space
extern "C" int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
		// i++;
        if (*format == '%') {
            format++;   // skip '%'
            switch (*format) {
				case 'u':
                case 'd': {
                    int i = va_arg(args, int);
                    _putInt(i);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char*);
                    if (s)
                        puts(s);
                    else
                        puts("(null)");
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    _putchar(c);
                    break;
                }
                case '%': {
                    _putchar('%');
                    break;
                }
                default:
                    // Unknown specifier: print '%' and the unknown character
                    _putchar('%');
                    _putchar(*format);
                    break;
            }
            format++;   // move past the specifier
        } else {
            // Regular character
            _putchar(*format);
            format++;
        }
    }

    va_end(args);
	return 0;
}