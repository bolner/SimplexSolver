#include <stdio.h>
#include <stdarg.h>
#include "exception.h"

/**
 * Constructor
 * Use the same parameters as for the printf() function.
 *
 * @param error_msg 
 * @param ... Variable argument list.
 * @returns FException
 */
FException::FException(char *error_msg, ...) {
	va_list arg_list;

	va_start(arg_list, error_msg);
	vsprintf_s(this->error_msg, (size_t)FE_MESSAGE_BUFFER_SIZE, error_msg, arg_list);
	va_end(arg_list);

	this->error_code = 0;
}

/**
 * Constructor
 * After the error_code use the same parameters as for the printf() function.
 * 
 * @param error_code
 * @param error_msg 
 * @param ... Variable argument list.
 * @returns FException
 */
FException::FException(unsigned __int64 error_code, char *error_msg, ...) {
	va_list arg_list;

	va_start(arg_list, error_msg);
	vsprintf_s(this->error_msg, (size_t)FE_MESSAGE_BUFFER_SIZE, error_msg, arg_list);
	va_end(arg_list);

	this->error_code = error_code;
}

/**
 * Prints out the error message text to the console.
 */
void FException::Print() {
	printf(this->error_msg);
}

/**
 * Returns the error code if it's set. Otherwise returns 0.
 * 
 * @returns unsigned __int64
 */
unsigned __int64 FException::getErrorCode() {
	return this->error_code;
}

/**
 * Returns the error message text of the exception.
 * 
 * @returns char*
 */
char* FException::getMessage() {
	return this->error_msg;
}
