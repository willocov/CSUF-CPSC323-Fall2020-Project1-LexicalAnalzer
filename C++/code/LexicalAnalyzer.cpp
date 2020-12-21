#include "LexicalAnalyzer.h"
#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <stdio.h>
using namespace std;

LexicalAnalyzer::LexicalAnalyzer() {
	stringBuilder = "";
	cachePosition = 0;
	currentState = NEUTRAL;
	recordPosition = 0;
}

void LexicalAnalyzer::LoadFile(string filename) {
	//This function: 
		//Opens a text file
		//Reads through file by char
		//Stores each into a queue
	try {
		cout << "Loading file: " << filename << endl << endl;
		char ch;
		fstream fin(filename, fstream::in);
		while (fin >> noskipws >> ch) {
			cout << ch; // Or whatever
			cache.push_back(ch);
		}
		fin.close();
		cout << endl << endl;
		//cout <<endl <<endl << "Finished Loading File" << endl;
	}
	catch(exception ex){
		cout << "Error: Failed to Load File.\n";
	}
}

void LexicalAnalyzer::PrintCache() {
	//cout << "Pringing Cache.\n";
	for (std::vector<char>::const_iterator iterator = cache.begin(), end = cache.end(); iterator != end; ++iterator) {
		cout << *iterator << endl;
	}
}

void LexicalAnalyzer::ParseRecords() {
	//This function parses out all records from the cache

	ClearStringBuilder();
	char ch;
	currentState = NEUTRAL;
	try {
		//Loop through every character in the file
		for (std::vector<char>::const_iterator iterator = cache.begin(), end = cache.end(); iterator != end; ++iterator) {
			ch = *iterator;

			//Process based on current state
			switch (currentState) {
			case NEUTRAL:
				ProcessNeutralState(ch);
				break;
			case COMMENT:
				ProcessCommentState(ch);
				break;
			case INTEGER:
				ProcessIntState(ch);
				break;
			case REAL:
				ProcessRealState(ch);
				break;
			case IDENTIFIER:
				ProcessIdentifierState(ch);
				break;
			}
		}
		ProcessLeftOverStringBuilder();
	}
	catch (exception ex) {
		cout << "Failed to Parse Records from Cache: " << ex.what() << endl;
	}
}

string LexicalAnalyzer::getString(char x) {
	string s(1, x);
	return s;
}

bool LexicalAnalyzer::IsOperator(char ch) {
	for (int i = 0; i < (sizeof(operators) / sizeof(operators[0])); i++) {
		if (ch == operators[i]) {
			return true;
		}
	}
	return false;
}

bool LexicalAnalyzer::IsSeparator(char ch) {
	for (int i = 0; i < (sizeof(seperators) / sizeof(seperators[0])); i++) {
		if (ch == seperators[i]) {
			return true;
		}
	}
	return false;
}

bool LexicalAnalyzer::IsKeyword(string word) {
	for (int i = 0; i < (sizeof(keywords) / sizeof(keywords[0])); i++) {
		if (word == keywords[i]) {
			return true;
		}
	}
	return false;

}

bool LexicalAnalyzer::IsComment(char ch) {
	if (ch == '!'){
		return true;	
	}
	else return false;
}

void LexicalAnalyzer::PrintAllLexors() {
	//cout << "Printing All Records" << endl;
	cout << "Indentifier \t Value" << endl;
	cout << "============================" << endl;
	
	for (vector<Record>::const_iterator it = records.begin(); it != records.end(); ++it) {
		cout << setw(15) << left << it->token << setw(15) << it->value << endl;
	}


}

bool LexicalAnalyzer::IsNumber(const std::string& s)
{
	try {
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}
	catch (exception ex) {
		cout << "Error: Failed to IsNumber: " << ex.what() << endl;
		return false;
	}
}

bool LexicalAnalyzer::IsNewline(char ch) {
	try {
		if (ch == '\n')
			return true;
		return false;
	}
	catch (exception ex) {
		cout << "Error: Failed to IsNewline: " << ex.what() << endl;
	}
}

bool LexicalAnalyzer::IsBool(string word) {
	if (word == "true" || word == "false") {
		return true;
	}
	return false;
}

void LexicalAnalyzer::AddSeparator(string word) {
	try {
		if (word == " ")
			word = "sp";
		word = TrimLeadingWhiteSpace(word);
		Record rec("Separator", word);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddSeparator: " << word << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddOperator(string word) {
	try {
		word = TrimLeadingWhiteSpace(word);
		Record rec("Operator", word);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddOperator: "<< word << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddKeyword(string word) {
	try {
		if (IsBool(word)) {
			AddBool(word);
		}
		else {
			Record rec("Keyword", word);
			records.push_back(rec);
		}
	}
	catch (exception ex) {
		cout << "Error: Failed to AddKeyword: " << word << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddIdentifier(string word) {
	try {
		word = TrimLeadingWhiteSpace(word);

		Record rec("Identifier", word);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddIdentifier: " << word << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddReal(string word) {
	try {
		word = TrimLeadingWhiteSpace(word);

		Record rec("Real", word);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddReal: " << word << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddInteger(string value) {
	try {
		value = TrimLeadingWhiteSpace(value);

		Record rec("Integer", value);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddInteger: " << value << " - " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddBool(string value) {
	try {
		value = TrimLeadingWhiteSpace(value);

		Record rec("Boolean", value);
		records.push_back(rec);
	}
	catch (exception ex) {
		cout << "Error: Failed to AddBool: " << ex.what() << endl;
	}
}

void LexicalAnalyzer::AddStringBuilder(char ch) {
	try {
		stringBuilder += ch;
	}
	catch (exception ex) {
		//Prints the char and its ascii value
		cout << "Error: Failed to AddStringBuilder: " << ch << " (" << (int)ch << ")" << endl;
	}
}

void LexicalAnalyzer::ClearStringBuilder() {
	stringBuilder = "";
}

void LexicalAnalyzer::ProcessNeutralState(char ch) {
	bool processString = false;		//Indicates whether to process string builder after operator, separator, etc
	bool processOperator = false;
	bool processSeparator = false;
				
	try {
		//Check if start of comment
		if (IsComment(ch)) {
			currentState = COMMENT;
		}
		else if (IsNewline(ch) || ch == '\t') {
			
		}
		else if (IsOperator(ch)) {
			//The char is an operator
			processString = true;
			processOperator = true;
		}
		else if (IsSeparator(ch)) {
			//The char is a separator
			processString = true;
			processSeparator = true;
		}
		else if (stringBuilder == "" && isdigit(ch)) {
			//The char is the begining of an int or real
			currentState = INTEGER;
		}
		else {
			currentState = IDENTIFIER;
		}

		//Determine whether to add the string to the string builder, or process string
		if (!processString && ch != '\n' && ch != '!' && ch != '\t') {
			//If we are not processing the string builder this iteration, add string to string builder
			AddStringBuilder(ch);
		}
		else {
			if (stringBuilder != "" && ch != '!' && ch != '\t') {

				ProcessStringBuilder();
				ClearStringBuilder();
			}
			
		}

		//Check whether to process operator or separartors
		if (processOperator) {
			AddOperator(getString(ch));
			currentState = NEUTRAL;
		}
		else if (processSeparator) {
			AddSeparator(getString(ch));
			currentState = NEUTRAL;
		}
	}
	catch(exception ex){
		cout << "Error: Failed to Process Neutral State: " << ex.what() << endl;
	}
}
void LexicalAnalyzer::ProcessCommentState(char ch) {
	//Check if input char is end comment char
	if (IsComment(ch)) {
		//It is a comment char, indicating the end of the comment
		if (stringBuilder == "")
		{
			currentState = NEUTRAL;
		}
		else { currentState = IDENTIFIER; }
	}
	//else skip the char because its a comment
}
void LexicalAnalyzer::ProcessIntState(char ch) {
	//Check if decimal point
	if (ch == '.') {
		//It is a decimal point. Change state to REAL
		AddStringBuilder(ch);
		currentState = REAL;
	}
	else if (IsNewline(ch)) {
		AddInteger(stringBuilder);
		ClearStringBuilder();
		currentState = NEUTRAL;
	}
	else if (isdigit(ch)) {
		//The char is another number for the integer
		AddStringBuilder(ch);
	}
	else if (IsSeparator(ch)) {
		//Process string builder and add separator token
		AddInteger(stringBuilder);
		ClearStringBuilder();
		AddSeparator(getString(ch));
		currentState = NEUTRAL;
	}
	else if (IsOperator(ch)) {
		AddInteger(stringBuilder);
		ClearStringBuilder();
		AddOperator(getString(ch));
		currentState = NEUTRAL;
	}
	else {
		cout << "Error: Invalid Char Detected in Integer State: " << ch << endl;
		cout << "StringBuilder: " << stringBuilder << endl;
	}
}
void LexicalAnalyzer::ProcessRealState(char ch) {
	if (isdigit(ch)) {
		AddStringBuilder(ch);
	}
	else if (IsNewline(ch)) {
		AddReal(stringBuilder);
		ClearStringBuilder();
		currentState = NEUTRAL;
	}
	else if (IsOperator(ch)) {
		//Process string builder and add operator token
		AddReal(stringBuilder);
		ClearStringBuilder();
		AddOperator(getString(ch));
		currentState = NEUTRAL;
	}
	else if (IsSeparator(ch)) {
		AddReal(stringBuilder);
		ClearStringBuilder();
		AddSeparator(getString(ch));
		currentState = NEUTRAL;
	}
	else {
		cout << "Error: Invalid Char Detected in REAL State: " << ch << endl;
		cout << "StringBuilder: " << stringBuilder << endl;
	}
}
void LexicalAnalyzer::ProcessIdentifierState(char ch) {
	if (IsOperator(ch)) {
		//Process string builder and add operator token
		ProcessStringBuilder();
		ClearStringBuilder();
		AddOperator(getString(ch));
		currentState = NEUTRAL;
	}
	else if (IsSeparator(ch)) {
		ProcessStringBuilder();
		ClearStringBuilder();
		AddSeparator(getString(ch));
		currentState = NEUTRAL;
	}
	else if (IsNewline(ch)) {
		ProcessStringBuilder();
		ClearStringBuilder();
		currentState = NEUTRAL;
	}
	else {
		AddStringBuilder(ch);
	}
}

void LexicalAnalyzer::ProcessStringBuilder() {
	//Check if string builder is a keyword
	if (IsKeyword(stringBuilder)) 
		AddKeyword(stringBuilder);
	else 
		//string builder is an identifier
		AddIdentifier(stringBuilder);
}

void LexicalAnalyzer::WriteFile(string outputFile, Record record) {
	ofstream myfile;
	myfile.open(outputFile, fstream::app);
	myfile << setw(15) << left << record.token << setw(15) << record.value << endl;
	myfile.close();
}

Record LexicalAnalyzer::Lexor() {
	try {
		Record temp = records[recordPosition++];
		return temp;
	}
	catch (exception ex) {
		cout << "Error: Failed to Lexor: " << ex.what() << endl;
		Record failed;
		return failed;

	}
}

void LexicalAnalyzer::PrintLexor(Record rec) {
	try {
		cout << setw(15) << left << rec.token << setw(15) << rec.value << endl;
	}
	catch (exception ex) {
		cout << "Error: Failed to PrintLexor: " << ex.what() << endl;
	}
}

void LexicalAnalyzer::PrintHeader() {
	cout << "Indentifier \t Value" << endl;
	cout << "============================" << endl;
}

void LexicalAnalyzer::WriteLexor(string filename, Record rec) {
	try {
		ofstream myfile;
		myfile.open(filename, fstream::app);	//Creates file if it does not exists, or appends existing file
		myfile << setw(15) << left << rec.token << setw(15) << rec.value << endl;		
		myfile.close();
	}
	catch (exception ex) {
		cout << "Error: Failed to WriteLexor: " << ex.what() << endl;
	}
}

void LexicalAnalyzer::WriteHeader(string filename) {
	try {
		remove(filename.c_str());
		
		ofstream myfile;
		myfile.open(filename, fstream::app);	//Creates file if it does not exists, or appends existing file
		myfile << "Indentifier \t Value" << endl;
		myfile << "============================" << endl;
		myfile.close();
	}
	catch (exception ex) {
		cout << "Error: Failed to WriteHeader: " << endl;
	}
}

void LexicalAnalyzer::ProcessLeftOverStringBuilder()  {
	try {
		switch (currentState) {
		case INTEGER:
			AddInteger(stringBuilder);
			break;
		case REAL:
			AddReal(stringBuilder);
			break;
		case IDENTIFIER:
			if (IsKeyword(stringBuilder)) 
				AddKeyword(stringBuilder);
			else 
				AddIdentifier(stringBuilder);
			break;
		}
		ClearStringBuilder();
		
	}
	catch (exception ex) {
		cout << "Error: Failed to ProcessLeftOverStringBuilder: " << ex.what() << endl;
	}
}

std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	return ltrim(rtrim(str, chars), chars);
}


string LexicalAnalyzer::TrimLeadingWhiteSpace(string input) {
	return ltrim(input);
}