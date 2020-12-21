#pragma once
#include <string>
#include <vector>
using namespace std;

const char seperators[] = { '\'', '(', ')', '{', '}', '[', ']', ',', '.', ':', ';', ' '};
const string keywords[] = { "int", "float", "bool", "true", "false", "if", "else", "then", "endif", "while", "whileend", "do", "doend", "for", "forend", "input", "output","and", "or", "not"};
const char operators[] = {'*', '+', '-', '=', '/', '>', '<', '%'};
enum STATE {NEUTRAL, COMMENT, SEPARATORS, OPERATORS, INTEGER, REAL, KEYWORD, IDENTIFIER};

struct Record {
	//This structure is used for storing a record (Token and Value).
	Record() {};
	Record(string t, string v) { token = t; value = v; };
	string token;
	string value;
};

class LexicalAnalyzer
{
	public:
		LexicalAnalyzer();
		//Functions related to file
		void LoadFile(string);
		void WriteFile(string, Record);

		//Returns next token when needed
		Record Lexor();
		void ParseRecords();
	 
		bool IsSeparator(char);
		bool IsOperator(char);
		bool IsKeyword(string);
		bool IsComment(char);
		bool IsNumber(const std::string& s);
		bool IsNewline(char);
		bool IsBool(string);

		void AddSeparator(string);
		void AddOperator(string);
		void AddKeyword(string);
		void AddIdentifier(string);
		void AddReal(string);
		void AddInteger(string);
		void AddBool(string);

		void ProcessNeutralState(char);
		void ProcessCommentState(char);
		void ProcessIntState(char);
		void ProcessRealState(char);
		void ProcessIdentifierState(char);
		void ProcessLeftOverStringBuilder();

		void AddStringBuilder(char);
		void ClearStringBuilder();
		void ProcessStringBuilder();


		//Test functions
		void PrintAllLexors();
		void PrintLexor(Record);
		void PrintHeader();
		void WriteLexor(string, Record);
		void WriteHeader(string);
		void PrintCache();
		string getString(char);
		string TrimLeadingWhiteSpace(string);
		vector<Record> records;

	private:
		vector<char> cache;
		string stringBuilder;
		int cachePosition;
		int currentState;

		int recordPosition;

};


