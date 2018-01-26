/*
*	lexer.c
*	Written by Micah Church 
*	C for gcc compiler
*	January, 25, 2018
*/
#include <ctype.h>
#include <stdio.h> //error checking
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
/*************************************************************************************
Includeding the header provided by Confer
*************************************************************************************/
#include "lexer.h"
/*************************************************************************************
Identify all of the possible tokens to be used in the correct order as specified by 
lexer.h
*************************************************************************************/
const char *id_string[] = {
	"TOKEN_VERSION",
	"TOKEN_NAME",
	"TOKEN_AUTHOR",
	"TOKEN_DRAWING",
	"TOKEN_REGION",
	"TOKEN_CIRCLE",
	"TOKEN_RECT",
	"TOKEN_KEYWORD_CNT", //!< used to limit keyword table search
	"TOKEN_BAD",         //!< unexpected character(s)
	"TOKEN_COMMENT",     //!< ignored whitespace and COMMENTS
	"TOKEN_IDENTIFIER",
	"TOKEN_NUMBER",
	"TOKEN_STRING",
	"TOKEN_POUND",
	"TOKEN_PERIOD",
	"TOKEN_EQUALS",
	"TOKEN_EOF"
};
/*************************************************************************************
Include all of the keywords discussed and from the handout in the proper order
*************************************************************************************/
const char *keyword[TOKEN_KEYWORD_CNT] = {
	"#Version",
	"#Name",
	"#Author",
	"#Drawing",
	"#Region",
	"#Circle",
	"#Rect"
};
/*************************************************************************************
Actual implementation of the code that is needed to detect the correct tokens to 
return
*************************************************************************************/
Token_ID get_token(char **buffer, Token *token) {
	
	char *begin = *buffer; // starting character of token
	char *end = NULL;      // end character of the token
	bool is_whitespace;    // bool to determine wheather ther is leading whitspace
	bool is_keyword;       // bool to say that this could be a keyword
	bool end_block = false;// bool to determine that the block comment is over
	

	/*
	 * Skip whitespace, if any, at the beginning of the token.
	 */
	is_whitespace = true;  // set to true to always look for whitespace
	is_keyword = false;    // assume that the keyword is not there
	
	while(is_whitespace) {
		switch(*begin) {
		case ' ':
		case '\n':
		case '\t':
		case '\b':
		case '\f':
		case '\v':
			begin++;
			break;
		default:
			is_whitespace = false;
			break;
		}
	}
	end = begin;
	switch(*end) {
/*************************************************************************************
Case to check if the first token is a pound, or a keyword
*************************************************************************************/	
	case '#':
		end++;
		if(isupper(*end)) {
			do {
				end++;
			} while(islower(*end));
			end--;
			is_keyword = true; //say this could be a keyword for use later on
		}
		else {
			end = begin;
			token->id = TOKEN_POUND;
		}
		break;
/*************************************************************************************
Case to check for STRING token
*************************************************************************************/
	case '[':
		end++;
			while(*end != ']' && *end != '\0') {
				end++;
			}
			if(*end == ']') {
				token->id = TOKEN_STRING;
			}
			else {
				end = begin;
				token->id = TOKEN_BAD;
			}
		break;
/*************************************************************************************
Case to check if it is the EOF token
*************************************************************************************/
	case '\0':
		token->id = TOKEN_EOF;
		break;
/*************************************************************************************
Case to check for EQUALS token
*************************************************************************************/
	case '=':
		token->id = TOKEN_EQUALS;
		break;
/*************************************************************************************
Case to check for PERIOD token
*************************************************************************************/
	case '.':
		token->id = TOKEN_PERIOD;
		break;
/*************************************************************************************
Case to check for COMMENT token
*************************************************************************************/
	case '*':
		end++;
		if(*end == '*') {
			end++;
			while(*end != '\0' && !end_block){
				if(*end == '*') {
					end++;
					if(*end == '*') {
						end_block = true;
					}
				}
				end++;
			}
			end--;
			if(!end_block) {
				end = begin;
				token->id = TOKEN_BAD;
				break;
			}
		}
/*------------------------------------------------------------------------------------
If it only has one astrix it must be a line comment
------------------------------------------------------------------------------------*/
		else {
			while(*end != '\0' && *end != '\n') {
				end++;
			}
			end--;
		}
		token->id = TOKEN_COMMENT;
		break;
	default:
/*************************************************************************************
Case to check for a NUMBER token
*************************************************************************************/		
		if(isdigit(*end) || *end == '+' || *end == '-') {
			end++;
			while(isdigit(*end)) {
				end++;
			}
			if(*end == '.') {
				end++;
				while(isdigit(*end)) {
					end++;
				}
				end--;
				if(!(isdigit(*end)))
					end--;
			}
			else	
				end--;
			token->id = TOKEN_NUMBER;
		} 
/*************************************************************************************
Case to check for IDENTIFIIER token
*************************************************************************************/
		else if (islower(*end)) {
			do {
				end++;
			} while(islower(*end)) ;
			end--;
			token->id = TOKEN_IDENTIFIER;
		}
		else {
			//end = begin;
			token->id = TOKEN_BAD;
		}
	}
		
	token->length = end - begin + 1;

	/*
	 * Copy the token string to the token.  The caller must free the string
	 * before AND after this function.  We add the '\0' since the token may
	 * be followed by additional characters.
	 */
	token->str = (char *)calloc(token->length + 1, sizeof(char)); //creat a dynamically allocated string to the exact size we need +1 filled with null characters
	/* Return a NULL string if allocation fails */
	if(token->str) {
		memcpy(token->str, begin, token->length); //copy the content of the string begin + length to str
		if(is_keyword) {
			int i = 0;
			while(i < TOKEN_KEYWORD_CNT) {
				if(strcmp(token->str, keyword[i]) == 0) {
					token->id = (Token_ID)i;
					break;
				}
				i++;
			}
			if (i == TOKEN_KEYWORD_CNT) {
				end = begin;
				token->id = TOKEN_POUND;
				token->str[1] = '\0';
			}
		}
	}

	*buffer = end + 1;

	return token->id;
}