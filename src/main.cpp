//#####################################################################
//#
//#	Author: Colton Staiduhar
//#	Date 8/28/2024
//# 
//#####################################################################
//#	
//#	This application was a project for a class in my Sophmore year of
//#	college.  The goal was to make a Wordle assistent Solver in python.
//#	At this time, I am very unfamiliar with python, so I had wrote an
//# automated form of the project in C++.
//#	
//#	The main.cpp file have a variable "useAutomation" to enable or
//#	disable the automated project.
//#	
//#	


#include <Wordle.hpp>
#include <time.h>
#include <cstdlib>
#include <string>
#include <WordleAutomation.hpp>

// Prints the user's guess to the screen and highlights correct letters
std::string PrintGuess(std::string, int);

// Demo the bot (true), play the game (false)
bool useAutomation = true;

// File Location, Defined in CMAKE
const char* wordsfile = RESOURCES_PATH "words.txt";

int main() {
	// Random Seed
	srand(time(0));

	if (useAutomation) {
		// Define some metrics for the bot
		int numOfGames = 10;

		int wins = 0;
		int fails = 0;

		for (int i = 0; i < numOfGames; ++i) {

			// Create the game
			Wordle wordle(wordsfile);

			// Stores guesses for bot, the bot will read the entire history on every guess
			std::string words[WORDLE_MAX_GUESSES];
			int			stats[WORDLE_MAX_GUESSES];

			// First Guess
			// Should a first guess yield zero letters, the next guess will not contain any of the same letters as the first
			{
				// Get guess word
				std::string guess = WAUTO::FirstGuess(wordsfile);

				// Get the guess status
				int fguess = wordle.Guess(guess.c_str());

				// Store guess in history
				words[wordle.RawRound() - 1] = guess;
				stats[wordle.RawRound() - 1] = fguess;

				// Print visual result
				std::cout << PrintGuess(guess, fguess) << "\n";
			}

			// Keep guessing untill the game is done or the board is full
			while (!wordle.Won() && !wordle.Done()) {
				// Make guess based on history
				std::string guess = WAUTO::MakeGuess(wordsfile, words, stats, wordle.RawRound());

				// Get the guess status
				int nguess = wordle.Guess(guess.c_str());

				// Store guess in history
				words[wordle.RawRound() - 1] = guess;
				stats[wordle.RawRound() - 1] = nguess;

				// Print visual result
				std::cout << PrintGuess(guess, nguess) << "\n";
			}

			// Win/Lose conditions
			if (wordle.Won()) {
				std::cout << "You Win!";
				++wins;
			} else {
				std::cout << "You lost :(";
				++fails;
			}

			// Clean the screen
			std::cout << "\n\n";
		}

		// Recap text
		std::cout << "\n\nAfter " << numOfGames << " the bot gained " << wins << " wins and " << fails << " fails.\n";
		std::cout << "A " << ((float)wins / (float)(wins + fails)) * 100 << "% ratio.\n\n";
		return 0;
	}
	

	// The Human Version

	// Create the game
	Wordle wordle(wordsfile);

	// Run until the game is won or board is full
	while (!wordle.Won() && !wordle.Done()) {
		
		// Message
		std::cout << "Please Enter a Guess: ";
		
		// User input
		std::string guess;
		std::cin >> guess;
		
		// Get guess result
		int nguess = wordle.Guess(guess.c_str());

		// Print visual result
		std::cout << PrintGuess(guess, nguess) << " - ";

	}

	// Win or Lose messages
	if (wordle.Won())
		std::cout << "You Win!";
	else
		std::cout << "You lost :(";

	return 0;
}

// Prints text to show the user their entry status 
std::string PrintGuess(std::string word, int n) {

	// Word Size
	int mws = WORDLE_WORD_SIZE;

	// Show Error if not valid
	if (n % 10 == 4) { std::string msg; for (int i = 0; i < WORDLE_WORD_SIZE; ++i) { msg += " "; } msg += std::string("   Word Size "); msg += std::to_string(mws); return msg; }

	// Print text with colors based on number
	std::string v = "";
	for (int i = WORDLE_WORD_SIZE - 1; i >= 0; --i) {
		int t = n % 10;
		if (t == 3) v = std::string("\033[1;32m") + word[i] + std::string("\033[0m") + v;
		if (t == 2) v = std::string("\033[1;33m") + word[i] + std::string("\033[0m") + v;
		if (t == 1) v = std::string("\033[1;37m") + word[i] + std::string("\033[0m") + v;
		n /= 10;
	}
	return v;
}