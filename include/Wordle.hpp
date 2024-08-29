//#####################################################################
//#
//#	Author: Colton Staiduhar
//#	Date 8/28/2024
//# 
//#####################################################################
//#	
//#	This file represents the wordle game. The code will select a
//#	random word from a text file and the user may make guesses as to
//#	figure out the word.  This program was created to make an algorithm
//#	that could solve this without human intervention.
//#
//# The program is not 100% correct. It makes error when down to pure
//# chance.  EX: fight, light, night, right, and sight.
//#	
//#	See wordle.cpp for the program code
//#	

#ifndef WORDLE
#define WORDLE

#include <iostream>

// Attempts
const int WORDLE_MAX_GUESSES	= 6;

// Size of words
const int WORDLE_WORD_SIZE		= 5;

class Wordle {
public:
	// Starts the game, inits everything, and selects a random word from the file
	Wordle(const char*);

	// Starts the game with a set word
	Wordle(std::string);

	/*
	* Returns a number of length WORDLE_WORD_SIZE
	* 1 - letter miss
	* 2 - letter present
	* 3 - letter correct
	* 4 - NULL
	* 
	* If the word is pizza and jazzy is entered,
	* the integer returned will be:
	*     12331
	*/
	int Guess(const char*);

	// Has the player one the game?
	bool Won()  const;

	// Is the game over?
	bool Done() const;
	
	// Game round counter
	int RawRound() const;

	// Human readable round counter, RawRound() + 1
	int Round() const;

	// Number of total rounds
	int Rounds() const;

	// Cleanup
	~Wordle();
private:
	// The selected word
	std::string word;

	// The number of user entries
	std::string entries[WORDLE_MAX_GUESSES];

	// The amount of rounds complete
	int rounds = 0;
};

#endif // !WORDLE