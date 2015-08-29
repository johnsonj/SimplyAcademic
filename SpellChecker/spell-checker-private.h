#ifndef __SPELL_CHECKER_PRIVATE_H
#define __SPELL_CHECKER_PRIVATE_H

#include <stdbool.h> 

/**
 * Determines if a single character is a valid stream character.
 * if it's not then it's considered a seperator.
 *
 * @param character
 *		Single ASCII 
 *
 * @return bool
 */
bool is_valid_char(
	const char character);

#endif