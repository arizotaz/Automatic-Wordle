//#####################################################################
//#
//#	Author: Colton Staiduhar
//#	Date 8/28/2024
//# 
//#####################################################################
//#	
//#	Code for Wordle.hpp.  For descriptions on function usage, see
//# Wordle.hpp
//#	

#include <Wordle.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> 


// Returns the number of lines in a file
int		NumOfLines(std::string&);

		Wordle::Wordle	(const char* filepath) {
	std::string& file = std::string(filepath);
	
	// Number of lines in the file
	int lines = NumOfLines(file);
	// pick a random line
	int randline = rand() % lines;

	// Open stream
	std::ifstream ifs(file);
	// Create Line Storage
	std::string tl;
	// Loop till word found
	int numOfLines = 0;
	std::getline(ifs, tl);
	while (ifs.good() && word.length() <= 0) {
		if (numOfLines == randline) {
			word = tl;
		}
		++numOfLines;
		std::getline(ifs, tl);
	}
	ifs.close();
	//std::cout << "Word is: " << word << "\n";
}
		Wordle::Wordle(std::string w) {
	word = w;
}
int		Wordle::Guess(const char* w) {
	// Checks if the entered word meets the size requirement, returns a string of 4's the size of WORDLE_WORD_SIZE if not met
	if (strlen(w) != WORDLE_WORD_SIZE) { int n = 0; for (int i = 0; i < WORDLE_WORD_SIZE; ++i) { n *= 10; n += 4; } return n; }

	// Returns the word state number
	int number = 0;
	for (int i = 0; i < WORDLE_WORD_SIZE; ++i) {
		number *= 10;
		int n = 1;
		if (word.find(w[i]) != std::string::npos) n = 2;
		if (word[i] == w[i]) n = 3;
		number += n;
	}

	// Adds text to entry list
	entries[rounds] = std::string(w);
	
	// Increment the round
	++rounds;
	return number;
}
bool	Wordle::Won		()  const { 
	return rounds > 0 && entries[rounds - 1] == word;
}
bool	Wordle::Done	()	const { 
	return rounds == WORDLE_MAX_GUESSES || Won();
}
int		Wordle::RawRound()	const {
	return rounds;
}
int		Wordle::Round	()	const {
	return RawRound() + 1;
}
int		Wordle::Rounds	()	const {
	return WORDLE_MAX_GUESSES;
}
		Wordle::~Wordle	()		  {

}

int NumOfLines(std::string& filepath) {
	int number_of_lines = 0;
	std::string line;
	std::ifstream fs(filepath);
	// As we count the number of page breaks, we must add one to the result to account for the final line.
	number_of_lines = std::count(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>(), '\n') + 1;
	fs.close();
	return number_of_lines;
}