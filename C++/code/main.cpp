//CPSC 323 
//Assignment 1 Lexical Analyzer
//William Covington

#include <iostream>
#include "LexicalAnalyzer.h";
using namespace std;

int main(int argc, char *argv[]) {
	
	//Exit program if input and output files are not passed in
	if (argc < 3) {
		cout << "Error: Not enough arguements." << endl;
		return 1;
	}

	//Save input and output files to strings
	string inputFile;
	string outputFile;
	try {
		inputFile = argv[1];
		outputFile = argv[2];
	}
	catch (exception ex) {
		cout << "Error: Failed to get arguements: " <<ex.what()<< endl;
		return 2;
	}

	LexicalAnalyzer lex = LexicalAnalyzer();
	try {
		lex.LoadFile(inputFile);	//Loads text from file into char cache
		lex.ParseRecords();			//Parses all Lexors out of input file and saves them to a vector of Records

		//Exit early if no lexors are parsed
		if (lex.records.empty()) {
			cout << "No Lexors Found In Records" << endl;
			return 0;
		}

		//Loop through Records and print/write Lexors
		lex.PrintHeader();
		lex.WriteHeader(outputFile);
		for (int i = 0; i < lex.records.size(); i++) {
			Record temp = lex.Lexor();
			lex.PrintLexor(temp);
			lex.WriteLexor(outputFile, temp);
		}
	}
	catch (exception ex) {
		cout << "Error: Lexical Analyzer Failed: " << ex.what() << endl;
	}
	return 0;
}