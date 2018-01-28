/*
*	Homework # 1
*	lexer.c
*	Written by Micah Church, Cynthia Ha and Luke Krentz 
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
/*************************************************************************************

Beigning the case to check the whole buffer for all of the correct tokens

1) Case to check for keyword token
	1.1) If not keyword its a pound token
2) Case to check for strings token
3) Case to check for comments token
	3.1) See if it is a line comment
4) Case to check for EOF token
5) Case to check for equals token
6) Case to check for period token
7) Case to check for number token
8) Case to check for identifier token
9) Compare the assumed keyword to the accepted keywords

*************************************************************************************/
	switch(*end) {
/*************************************************************************************
1) Case to check if the first token is a pound, or a keyword
*************************************************************************************/	
	case '#':
		end++;
		//All keywords start with an upper case letter so check for that
		if(isupper(*end)) {
			//Keyword then have all lower case letters
			do {
				end++;
			} while(islower(*end));
			end--;
			is_keyword = true; //say this could be a keyword for use later on
		}
		/*--------------------------------------------------------------------
		1.1) If the line has a pound but has anything other than Uppercase than 
		the token is a pound
		--------------------------------------------------------------------*/
		else {
			end = begin;
			token->id = TOKEN_POUND;
		}
		break;
/*************************************************************************************
2) Case to check for STRING token
*************************************************************************************/
	case '[':
		end++;
		//Go through all the preceeding characters until eof, ] or newline 
		while(*end != ']' && *end != '\0' && *end != '\n') {
			end++;
		}
		//If the last character is a ] it is a string
		if(*end == ']') {
			token->id = TOKEN_STRING;
		}
		//Otherwise the [ is a bad token and the rest is normal
		else {
			end = begin;
			token->id = TOKEN_BAD;
		}
		break;
/*************************************************************************************
3) Case to check for COMMENT token
*************************************************************************************/
	case '*':
		end++;
		//See if there is another * to check for block comment
		if(*end == '*') {
			end++;
			while(*end != '\0' && !end_block){
				//go through till there is a *
				if(*end == '*') {
					end++;
					//check for a block comment closing
					if(*end == '*') {
						end_block = true;
					}
				}
				end++;
			}
			end--;
			//if it doesnt end the first * is bad and then treat the rest as normal
			if(!end_block) {
				end = begin;
				token->id = TOKEN_BAD;
				break;
			}
		}
		/*--------------------------------------------------------------------
		3.1) If it only has one astrix it must be a line comment
		--------------------------------------------------------------------*/
		else {
			while(*end != '\0' && *end != '\n') {
				end++;
			}
			end--;
		}
		token->id = TOKEN_COMMENT;
		break;
/*************************************************************************************
4) Case to check if it is the EOF token
*************************************************************************************/
	case '\0':
		token->id = TOKEN_EOF;
		break;
/*************************************************************************************
5) Case to check for EQUALS token
*************************************************************************************/
	case '=':
		token->id = TOKEN_EQUALS;
		break;
/*************************************************************************************
6) Case to check for PERIOD token
*************************************************************************************/
	case '.':
		token->id = TOKEN_PERIOD;
		break;
	default:
/*************************************************************************************
7) Case to check for a NUMBER token
*************************************************************************************/		
		if(isdigit(*end) || *end == '+' || *end == '-') {
			end++;
			while(isdigit(*end)) {
				end++;
			}
			//See if it is a real number 
			if(*end == '.') {
				end++;
				while(isdigit(*end)) {
					end++;
				}
				end--;
				//if it is a real number with another . decriment end one so it is 
				//a valid number
				if(!(isdigit(*end)))
					end--;
			}
			//if it is not real then get rid of the .
			else	
				end--;
			token->id = TOKEN_NUMBER;
		} 
/*************************************************************************************
8) Case to check for IDENTIFIIER token
*************************************************************************************/
		//go until there isnt a lower case letter
		else if (islower(*end)) {
			do {
				end++;
			} while(islower(*end)) ;
			end--;
			token->id = TOKEN_IDENTIFIER;
		}
		//if its not an identifier it is a bad token
		else {
			token->id = TOKEN_BAD;
		}
	}
	token->length = end - begin + 1;

	/*
	 * Copy the token string to the token.  The caller must free the string
	 * before AND after this function.  We add the '\0' since the token may
	 * be followed by additional characters.
	 */
	//create a dynamically allocated string to the exact size we need +1 filled 
	//with null characters
	token->str = (char *)calloc(token->length + 1, sizeof(char)); 
	/* Return a NULL string if allocation fails */
	if(token->str) {
		//Copy the substring from begin to the length to token->str
		memcpy(token->str, begin, token->length);
/*************************************************************************************
9) Check to see if the assumed keyword is actually a keyword
*************************************************************************************/
		if(is_keyword) {
			int i = 0;
			//cycle through all accepted keywords till it matches then break
			while(i < TOKEN_KEYWORD_CNT) {
				if(strcmp(token->str, keyword[i]) == 0) {
					token->id = (Token_ID)i;
					break;
				}
				i++;
			}
			//if the loop counter is = to TOKEN_KEYWORD_CNT then it isnt a key word
			//so it must be a pound token
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
