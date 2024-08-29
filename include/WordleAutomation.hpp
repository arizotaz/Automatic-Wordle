//#####################################################################
//#
//#	Author: Colton Staiduhar
//#	Date 8/28/2024
//# 
//#####################################################################
//#	
//#	This file hold the code for guessing in the automated wordle
//# program. This file makes and keeps track of guesses.
//#
//# To see how the system communicates with the program see main.cpp
//#	

#ifndef AUTOWORDLE
#define AUTOWORDLE

#include <iostream>
#include <string>
#include <fstream>

namespace WAUTO {

	// Display bot debug info
	bool BOT_DEBUG = false;

	// Prototypes
	bool		HasRepeatingLetters(std::string);
	int*		ListCharacterCounts(const char*);
	int			GetHighestWordScore(const char*, int*, std::string);
	std::string GetHighestScoreText(const char*, int*, std::string, int);

	// List of valid letters (DEBUGGING)
	const char* letters = "abcdefghijklmnopqrstuvwxyz";

	/* A function to make an educated first word guess based on the commonality of words in the source document. 
	   Returns a single string.  A word containing the most common letters in the document without duplicate
	   characters. */
	std::string FirstGuess(const char* filepath) {

		// Store the final result
		std::string bestWord;

		// List of words to ignore
		std::string ignoreList;

		// Count every character in the file and get occurences
		int* charactermap = ListCharacterCounts(filepath);

		// loop until a word is found
		while (bestWord.length() <= 0) {

			// Find the score of a word, score is by the sum of the occurance in the file of each character in the string
			int maxScore = GetHighestWordScore(filepath, charactermap, ignoreList);

			// Finds a word with that score
			bestWord	 = GetHighestScoreText(filepath, charactermap, ignoreList, maxScore);

			// Debug
			if (BOT_DEBUG) std::cout << "Best Scoring Word: " << bestWord << "\n";
			if (BOT_DEBUG) std::cout << "Repeating Letters: " << HasRepeatingLetters(bestWord) << "\n";

			// Check for repeats
			if (HasRepeatingLetters(bestWord)) {
				// Add to ignore list
				ignoreList += bestWord + ",";

				// Reset final word
				bestWord = "";
			}
		}
		
		return bestWord;
	}

	/* Reads an entire file, and returns the occurance of every letter. */
	int* ListCharacterCounts(const char* filepath) {
		// Create Array
		int* charactermap = new int[255] { 0 };

		// Load File
		std::ifstream fs(filepath);

		// Read every character
		char c;
		fs >> c;
		while (fs.good()) {

			// Add 1 at every occurance to the integer index of char
			++charactermap[(int)c];

			// Read next character
			fs >> c;
		}

		// Debug, looks cool in terminal if uncommented.
		for (int i = 0; i < strlen(letters); ++i) {
			if (BOT_DEBUG) std::cout << letters[i] << " - " << charactermap[letters[i]] << "\n";
		}

		// Close the Stream
		fs.close();

		return charactermap;
	}
	/* Reads all words and returns the highest score not in ignorelist */
	int GetHighestWordScore(const char* filepath, int* charactermap, std::string ignoreList) {
		int maxScore = 0;
		std::ifstream fs(filepath);
		std::string line;
		std::getline(fs, line);
		while (fs.good()) {
			int l = line.length();
			int score = 0;
			for (int i = 0; i < l; ++i)
				score += charactermap[line[i]];
			if (score > maxScore && ignoreList.find(line + ",") == std::string::npos) maxScore = score;
			std::getline(fs, line);
		}
		fs.close();
		return maxScore;
	}
	/* Gets the word that corresponds to a given score not in ignorelist */
	std::string GetHighestScoreText(const char* filepath, int* charactermap, std::string ignoreList, int maxScore) {
		std::string bestWord;
		// Find the line with the score
		std::ifstream fs(filepath);
		std::string line;
		std::getline(fs, line);
		while (fs.good() && bestWord.length() <= 0) {
			int l = line.length();
			int score = 0;
			for (int i = 0; i < l; ++i)
				score += charactermap[line[i]];
			if (score == maxScore && ignoreList.find(line + ",") == std::string::npos)
				bestWord = line;
			std::getline(fs, line);
		}
		fs.close();
		return bestWord;
	}

	

	/* Returns a line from the document that would best fit as the next guess.
	This function takes a history of past actions as well as the current round and word.txt file path.*/
	std::string MakeGuess(const char* filepath,std::string words[WORDLE_MAX_GUESSES], int stats[WORDLE_MAX_GUESSES], int round) {

		// Does the bot know there are solid(green) characters
		bool hasSL = false;

		// List of correct characters
		char solidLetters[WORDLE_WORD_SIZE] = { '\0' };

		// List of non solid characters, but are contained in the word
		std::string nsolidLetters[WORDLE_WORD_SIZE] = { "" };

		// List of letters that must be in the next guess
		std::string contents = "";

		// List of letters to never include in further guesses
		std::string ncontents = "";

		// The final result
		std::string guess;

		// Read the history and contruct the values above based on each round
		for (int r = 0; r < round; ++r) {
			int stat		 = stats[r];
			std::string word = words[r];

			// Reads back to fron to accomodate for that state variable
			for (int l = WORDLE_WORD_SIZE - 1; l >= 0; --l) {

				// Get the last digit of the state
				int s = stat % 10;
				// Save the char for that state
				char c = word[l];

				// If the character is green, add it to solid letters list and set the flag to useSolidLetters to true
				if (s == 3) { solidLetters[l] = c; hasSL = true; }

				// If the character is yellow
				if (s == 2) {
					// Add the character to need requirements if not already there
					if (contents.find(c) == std::string::npos) 
						contents += c;
					// Stop that character from showing in that position again
					if (nsolidLetters[l].find(c) == std::string::npos)
						nsolidLetters[l] += c;
				}
				// Ban the character from showing in other guesses
				if (s == 1) if (ncontents.find(c) == std::string::npos) ncontents += c;

				// Pop off the last digit of the state
				stat /= 10;
			}
		}

		// Debug
		if (BOT_DEBUG) std::cout << "Guess must contain: " << contents << " and exclude:" << ncontents << "\n";

		/*
		The following block is responsible for the login of guesses, following the first guess.
		The bot will look over every word until one meets the criteria.
		1. A word must contain the NEED letters
		2. A word must not contain exclude letters
		3. A word must not have letters in the nonSolidLetter array that is not to be there
		4. A word must have the correct letters in the solidLetter array if they have been found
		*/

		{
			// Read the file line by line
			std::ifstream fs(filepath);
			std::string line;
			std::getline(fs, line);
			while (fs.good() && guess.length() <= 0) {
				
				// Get word length
				int ls = line.length();

				// Check if the word was already attempted
				bool tried = false;
				for (int i = 0; i < round; ++i) {
					if (line == words[i]) tried = true;
				}
				
				if (!tried) {
					// The word contains all required characters
					bool containsContents = true;
					for (int c = 0; c < contents.length(); ++c) {
						char con = contents[c];
						if (line.find(con) == std::string::npos) containsContents = false;
					}
					if (containsContents) {
						// The word does not contain any excluded characters
						bool excludeContents = true;
						for (int c = 0; c < ncontents.length(); ++c) {
							char con = ncontents[c];
							if (line.find(con) != std::string::npos) excludeContents = false;
						}
						if (excludeContents) {
							// The word does not contain letters in known bad positions
							bool validNSL = true;
							for (int s = 0; s < WORDLE_WORD_SIZE; ++s) {
								char c = line[s];
								if (nsolidLetters[s].find(c) != std::string::npos) {
									validNSL = false;
								}
							}
							if (validNSL) {
								// If there are know letter locations
								if (hasSL) {
									// The word has correct know letter locations
									bool validSL = true;
									for (int s = 0; s < WORDLE_WORD_SIZE; ++s) {
										if (solidLetters[s] != '\0')
											if (line[s] != solidLetters[s])
												validSL = false;
									}
									// The word is a good guess
									if (validSL) guess = line;
								} else {
									// The word is a good guess and may lead to correct letters
									guess = line;
								}
							}
						}
					}
				}

				// Read next line
				std::getline(fs, line);
			}

			// Close file
			fs.close();
		}

		return guess;
	}

	// Checks a word for repeating characters
	bool HasRepeatingLetters(std::string str) {
		bool value = false;
		int charactermap[255] = { 0 };
		int l = str.length();
		for (int i = 0; i < l; ++i) {
			++charactermap[str[i]];
			if (charactermap[str[i]] > 1) value = true;
		}

		return value;
	}
}
#endif // !AUTOWORDLE
