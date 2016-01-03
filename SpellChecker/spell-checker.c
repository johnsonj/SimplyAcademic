//
// spell-checker.c by Jeff Johnson <johnsonjeff@gmail.com>
//
// Tested on Visual Studio CTP 2015 (x64) with _CRT_SECURE_NO_WARNINGS enabled
// Tested on Apple Clang 6.1 (x64)
//
// On an Ivy Bridge i7 this can process 3.5 MB of in memory text with a 2.6 MB dictionary
// in around 100ms CPU/Wall, or ~35 MBps.
//

#include "spell-checker.h"
#include <stddef.h>
#include <stdbool.h> 
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


//
// Data Structures
//

//
// DictionaryNode is responsible for representing the internal trie structure of the dictionary.
// The first_child points to the next lower level and the next pointer goes to nodes sibblings.
//
// Given a dictionary with hey, hi, heynow, wow, we will have the following DictionaryNodes
//      root
//       |                     LEGEND =========
//       h-----(i)-----w       -  next
//       |             |       |  first_child
//       e             o      (X) end of word
//       |             |
//      (y)           (w)
//       |
//       n
//       |
//       o
//       |
//      (w)

struct DictionaryNode
{
	char value;
	bool end_of_word;
	struct DictionaryNode* next;
	struct DictionaryNode* first_child;
};

typedef struct DictionaryNode DictionaryNode_t;

//
// _SpellCheckerDictionary is typedef'd to DictionaryNode to allow for further definition of
// the root node. It may be useful to add statistics or locking at the root node.
// This can be acomplished with defining struct _SpellCheckerDictionary with an anonymous 
// struct of DictionaryNode followed by the other data:
//
// struct _SpellCheckerDictionary
// {
//		struct DictionaryNode;
//		// .. additional data
// };
//
typedef struct DictionaryNode _SpellCheckerDictionary;


//
// Dictionary Lifecycle
//
SpellCheckerDictionaryHandle createSpellCheckerDictionary()
{
	SpellCheckerDictionaryHandle dictionaryHandle = calloc(sizeof(_SpellCheckerDictionary), sizeof(_SpellCheckerDictionary));

	return dictionaryHandle;
}

void free_children(DictionaryNode_t* node)
{
	if (node == NULL)
		return;
	
	free_children(node->first_child);
	free_children(node->next);
	free(node);
}

int closeSpellCheckerDictionary(SpellCheckerDictionaryHandle dict)
{
	if (dict == NULL)
		return -1;

	free_children(((DictionaryNode_t*)dict)->first_child);
	free(dict);

	return 0;
}

bool is_valid_char(unsigned char character)
{
	// characters that aren't in range:
	if (character < '0')
		return false;

	if (character > '9' && character < 'A')
		return false;

	if (character > 'Z' && character < 'a')
		return false;

	if (character > 'z' && character < 0x80)
		return false;

	return true;
}

// A-Z is treated as a-z
char sanitize_value(char value)
{
	assert(is_valid_char(value));

	if (value >= 'A' && value <= 'Z')
		return tolower(value);

	return value;
}

// Do all comparisons with this method to ensure we're always comparing the correct value
bool node_equals_value(const DictionaryNode_t* node, char value)
{
	return node->value == sanitize_value(value);
}


//
// Dictonary population functions
//
DictionaryNode_t* create_node(char value)
{
	DictionaryNode_t* node = calloc(sizeof(DictionaryNode_t), sizeof(DictionaryNode_t));
	if (node == NULL)
		return NULL;

	node->value = sanitize_value(value);

	return node;
}

int spellCheckerAddWord(SpellCheckerDictionaryHandle dict, const char *word)
{
	if (dict == NULL)
		return -1;

	DictionaryNode_t *current_level = (DictionaryNode_t*)dict;
	DictionaryNode_t *current_sibbling = NULL;
	int wordPos = 0;
	size_t wordLen = strlen(word);

	for (size_t i = 0; i < wordLen; i++)
	{
		// Don't add invalid characters. If we fail this add we will leave the tree partially populated
		// with this invalid word, but it will not put the tree in an invalid state as it's not marked
		// as end_of_word
		if (!is_valid_char(word[i]))
			return -1;

		// Empty children, create a new child DictionaryNode for this char
		if (current_level->first_child == NULL)
		{
			current_level->first_child = create_node(word[i]);

			if (current_level->first_child == NULL)
				return -1;

			// Traverse down to the new DictionaryNode
			current_sibbling = current_level = current_level->first_child;
		}
		else
		{
			// Head down the children and look for a match
			current_sibbling = current_level->first_child;

			// Go until the end OR until we find what we're looking for
			while (current_sibbling->next != NULL && !node_equals_value(current_sibbling, word[i]))
				current_sibbling = current_sibbling->next;

			// No match, create the DictionaryNode
			if (!node_equals_value(current_sibbling, word[i]))
			{
				// This could be sped up by keeping the next list in order
				current_sibbling->next = create_node(word[i]);;

				if (current_sibbling->next == NULL)
					return -1;
				
				current_sibbling = current_sibbling->next;
			}
			current_level = current_sibbling;
		}
	}

	current_sibbling->end_of_word = true;
	return 0;
}


//
// Spell checking functions
//
int handle_invalid_word(const char *start, const char *end, SpellCheckerCallback callback)
{
	size_t res_length = end - start;
	char *res = malloc(res_length + 1);

	// Out of memory?
	if (res == NULL)
		return -1;

	// Copy the invalid string
	strncpy(res, start, end - start);

	// Null terminate it
	res[res_length] = '\0';

	// Change all upper case letters to lower case
	for (size_t i = 0; i < res_length; i++)
	{
		if (res[i] >= 'A' && res[i] <= 'Z')
			res[i] = tolower(res[i]);
	}

	callback(res);
	
	free(res);

	return 0;
}

// Helpers to make spellCheck read more clearly
bool made_word_progress(const char *word_start, const char *word_current)
{
	return word_current != word_start;
}

bool valid_word_end(const DictionaryNode_t* node)
{
	return node != NULL && node->end_of_word;
}

bool stream_has_data(const char *word_current)
{
	return *word_current != '\0';
}

void spellCheck(SpellCheckerDictionaryHandle dict, const char *text, SpellCheckerCallback callback)
{
	if (dict == NULL || text == NULL)
		return;

	const char *word_current, *word_start;
	word_current = word_start = text;

	DictionaryNode_t *node = (DictionaryNode_t*)dict;

	while (stream_has_data(word_current))
	{
		// Seek until we hit a word
		while (!is_valid_char(*word_current) && stream_has_data(word_current))
			word_start = ++word_current;

		// Search through the current word
		word_start = word_current;
		node = (DictionaryNode_t*)dict;
		while (is_valid_char(*word_current))
		{
			// Traverse the nodes a level to try and match the current character with node
			// If we reach a dead end, node will be NULL. Continue to loop through the stream
			// till we reach the end of the word.
			if (node != NULL)
			{
				node = node->first_child;

				// Iterate through the children and look for a value.
				// If we don't find one, node will be NULL
				while (node != NULL && !node_equals_value(node, *word_current))
					node = node->next;
			}

			++word_current;
		}

		// Was it valid for us to end here? If not execute the callback
		if (made_word_progress(word_start, word_current) && !valid_word_end(node))
		{
			handle_invalid_word(word_start, word_current, callback);
		}
	}
}