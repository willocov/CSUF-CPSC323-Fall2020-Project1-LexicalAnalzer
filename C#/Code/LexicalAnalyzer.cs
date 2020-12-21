using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Numerics;
using System.Reflection;
using System.Text;

namespace LexicalAnalyzer
{

	public struct Record
	{
		public Record(string t, string v) { token = t; value = v; }
		public string token;
		public string value;
	};

	public class LexicalAnalyzer
	{

		char[] seperators = { '\'', '(', ')', '{', '}', '[', ']', ',', '.', ':', ';', ' ' };
		string[] keywords = { "int", "float", "bool", "true", "false", "if", "else", "then", "endif", "while", "whileend", "do", "doend", "for", "forend", "input", "output", "and", "or", "not" };
		char[] operators = { '*', '+', '-', '=', '/', '>', '<', '%' };
		enum STATE { NEUTRAL, COMMENT, SEPARATORS, OPERATORS, INTEGER, REAL, KEYWORD, IDENTIFIER };

		public List<char> cache;
		public string stringBuilder;
		public int cachePosition;
		public int currentState;
		public List<Record> records;
		public int recordPosition;

		public LexicalAnalyzer() {
			stringBuilder = "";
			cachePosition = 0;
			currentState = (int)STATE.NEUTRAL;
			recordPosition = 0;
			cache = new List<char>();
			records = new List<Record>();
		}

		public void LoadFile(string filename) {
			//This function: 
			//Opens a text file
			//Reads through file by char
			//Stores each into a queue
			try
			{

				filename = Path.Combine(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location), filename);
				Console.WriteLine("Loading file: " + filename);
				using (StreamReader sr = new StreamReader(filename))
				{
					while (sr.Peek() >= 0)
					{
						char ch = (char)sr.Read();
						Console.Write(ch);
						cache.Add(ch);
					}
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to Load File: " + ex.Message);
			}
		}

		public void WriteFile(string filename, Record rec) {
			using (StreamWriter sw = new StreamWriter(filename)) {
				sw.WriteLine(rec.token.PadRight(15) + rec.value);
			}
		}

		public Record Lexor() {
			try
			{
				Record temp = records[recordPosition++];
				return temp;
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to Lexor: " + ex.Message);
				return new Record();

			}
		}

		public void ParseRecords() {
			//This function parses out all records from the cache

			ClearStringBuilder();
			
			currentState = (int)STATE.NEUTRAL;
			try
			{
				//Loop through every character in the file
				foreach(char ch in cache)
				{
					
					//Process based on current state
					switch (currentState)
					{
						case (int)STATE.NEUTRAL:
							ProcessNeutralState(ch);
							break;
						case (int)STATE.COMMENT:
							ProcessCommentState(ch);
							break;
						case (int)STATE.INTEGER:
							ProcessIntState(ch);
							break;
						case (int)STATE.REAL:
							ProcessRealState(ch);
							break;
						case (int)STATE.IDENTIFIER:
							ProcessIdentifierState(ch);
							break;
					}
				}
				ProcessLeftOverStringBuilder();
			}
			catch (Exception ex)
			{
				Console.WriteLine( "Failed to Parse Records from Cache: " + ex.Message);
			}
		}

		public bool IsSeparator(char ch) {
			foreach (char separator in seperators)
			{
				if (ch == separator)
					return true;
			}
			return false;
		}

		public bool IsOperator(char ch) {
			foreach (char op in operators)
				if (ch == op)
					return true;
			return false;
		}

		public bool IsKeyword(string word) {
			foreach (string keyword in keywords) {
				if (word == keyword)
					return true;	
			}
			return false;
		}

		public bool IsComment(char ch) {
			if (ch == '!')
				return true;
			return false;
		}

		public bool IsNumber(string word){
			int out1;
			float out2;
			if (int.TryParse(word, out out1) || float.TryParse(word, out out2))
				return true;
			return false;
			
		}

		public bool IsNewline(char ch) {
			if (ch == '\n')
				return true;
			return false;
		}

		public bool IsBool(string word) {
			if (word.ToLower() == "true" || word.ToLower() == "false")
				return true;
			return false;
		}

		public void AddSeparator(string word) {
			try
			{
				if (word == " ")
					word = "sp";
				word = TrimLeadingWhiteSpace(word);
				Record rec = new Record("Separator", word);
				records.Add(rec);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddSeparator: " + word + " - " + ex.Message);
			}
		}

		public void AddOperator(string word) {
			try
			{
				word = TrimLeadingWhiteSpace(word);
				Record rec = new Record("Operator", word);
				records.Add(rec);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddOperator: " + word + " - " + ex.Message);
			}
		}

		public void AddKeyword(string word) {
			try
			{
				if (IsBool(word))
				{
					AddBool(word);
				}
				else
				{
					Record rec = new Record("Keyword", word);
					records.Add(rec);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddKeyword: " + word + " - " + ex.Message);
			}
		}

		public void AddIdentifier(string word) {
			try
			{
				word = TrimLeadingWhiteSpace(word);
				if (word != "")
				{
					Record rec = new Record("Identifier", word);
					records.Add(rec);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddIdentifier: " + word + " - " + ex.Message);
			}
		}

		public void AddReal(string word) {
			try
			{
				word = TrimLeadingWhiteSpace(word);

				Record rec = new Record("Real", word);
				records.Add(rec);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddReal: " + word + " - " + ex.Message);
			}
		}

		public void AddInteger(string value) {
			try
			{
				value = TrimLeadingWhiteSpace(value);

				Record rec = new Record("Integer", value);
				records.Add(rec);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddInteger: " + value + " - " + ex.Message);
			}
		}
		public void AddBool(string value) {
			try
			{
				value = TrimLeadingWhiteSpace(value);

				Record rec = new Record("Boolean", value);
				records.Add(rec);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to AddBool: " + value + " - " + ex.Message);
			}
		}

		public void ProcessNeutralState(char ch) {
			bool processString = false;     //Indicates whether to process string builder after operator, separator, etc
			bool processOperator = false;
			bool processSeparator = false;

			try
			{
				//Check if start of comment
				if (IsComment(ch))
				{
					currentState = (int)STATE.COMMENT;
				}
				else if (IsNewline(ch) || ch == '\t')
				{

				}
				else if (IsOperator(ch))
				{
					//The char is an operator
					processString = true;
					processOperator = true;
				}
				else if (IsSeparator(ch))
				{
					//The char is a separator
					processString = true;
					processSeparator = true;
				}
				else if (stringBuilder == "" && Char.IsDigit(ch))
				{
					//The char is the begining of an int or real
					currentState = (int)STATE.INTEGER;
				}
				else
				{
					currentState = (int)STATE.IDENTIFIER;
				}

				//Determine whether to add the string to the string builder, or process string
				if (!processString && ch != '\n' && ch != '!' && ch != '\t')
				{
					//If we are not processing the string builder this iteration, add string to string builder
					AddStringBuilder(ch);
				}
				else
				{
					if (stringBuilder != "" && ch != '!' && ch != '\t')
					{

						ProcessStringBuilder();
						ClearStringBuilder();
					}

				}

				//Check whether to process operator or separartors
				if (processOperator)
				{
					AddOperator(getString(ch));
					currentState = (int)STATE.NEUTRAL;
				}
				else if (processSeparator)
				{
					AddSeparator(getString(ch));
					currentState = (int)STATE.NEUTRAL;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to Process Neutral State: " + ex.Message);
			}
		}

		public void ProcessCommentState(char ch) {
			//Check if input char is end comment char
			if (IsComment(ch))
			{
				//It is a comment char, indicating the end of the comment
				if (stringBuilder == "")
				{
					currentState = (int)STATE.NEUTRAL;
				}
				else { currentState = (int)STATE.IDENTIFIER; }
			}
		}

		public void ProcessIntState(char ch) {
			//Check if decimal point
			if (ch == '.')
			{
				//It is a decimal point. Change state to REAL
				AddStringBuilder(ch);
				currentState = (int)STATE.REAL;
			}
			else if (IsNewline(ch))
			{
				AddInteger(stringBuilder);
				ClearStringBuilder();
				currentState = (int)STATE.NEUTRAL;
			}
			else if (Char.IsDigit(ch))
			{
				//The char is another number for the integer
				AddStringBuilder(ch);
			}
			else if (IsSeparator(ch))
			{
				//Process string builder and add separator token
				AddInteger(stringBuilder);
				ClearStringBuilder();
				AddSeparator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else if (IsOperator(ch))
			{
				AddInteger(stringBuilder);
				ClearStringBuilder();
				AddOperator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else
			{
				Console.WriteLine("Error: Invalid Char Detected in Integer State: " + ch.ToString());
				Console.WriteLine("StringBuilder: " + stringBuilder);
			}
		}
		public void ProcessRealState(char ch) {
			if (Char.IsDigit(ch))
			{
				AddStringBuilder(ch);
			}
			else if (IsNewline(ch))
			{
				AddReal(stringBuilder);
				ClearStringBuilder();
				currentState = (int)STATE.NEUTRAL;
			}
			else if (IsOperator(ch))
			{
				//Process string builder and add operator token
				AddReal(stringBuilder);
				ClearStringBuilder();
				AddOperator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else if (IsSeparator(ch))
			{
				AddReal(stringBuilder);
				ClearStringBuilder();
				AddSeparator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else
			{
				Console.WriteLine("Error: Invalid Char Detected in REAL State: " + ch.ToString());
				Console.WriteLine("StringBuilder: " + stringBuilder);
			}
		}
		public void ProcessIdentifierState(char ch) {
			if (IsOperator(ch))
			{
				//Process string builder and add operator token
				ProcessStringBuilder();
				ClearStringBuilder();
				AddOperator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else if (IsSeparator(ch))
			{
				ProcessStringBuilder();
				ClearStringBuilder();
				AddSeparator(getString(ch));
				currentState = (int)STATE.NEUTRAL;
			}
			else if (IsNewline(ch))
			{
				ProcessStringBuilder();
				ClearStringBuilder();
				currentState = (int)STATE.NEUTRAL;
			}
			else
			{
				AddStringBuilder(ch);
			}
		}
		public void ProcessLeftOverStringBuilder() {
			try
			{
				switch (currentState)
				{
					case (int)STATE.INTEGER:
						AddInteger(stringBuilder);
						break;
					case (int)STATE.REAL:
						AddReal(stringBuilder);
						break;
					case (int)STATE.IDENTIFIER:
						if (IsKeyword(stringBuilder))
							AddKeyword(stringBuilder);
						else
							AddIdentifier(stringBuilder);
						break;
				}
				ClearStringBuilder();

			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to ProcessLeftOverStringBuilder: " + ex.Message);
			}
		}

		public void AddStringBuilder(char ch) {
			try
			{
				stringBuilder += ch;
			}
			catch (Exception ex)
			{
				//Prints the char and its ascii value
				Console.WriteLine("Error: Failed to AddBool: " + ch.ToString() + " - " + ex.Message);
			}
		}
		public void ClearStringBuilder() {
			stringBuilder = "";
		}
		public void ProcessStringBuilder() {
			//Check if string builder is a keyword
			if (IsKeyword(stringBuilder))
				AddKeyword(stringBuilder);
			else
				//string builder is an identifier
				AddIdentifier(stringBuilder);
		}


		//Test functions
		public void PrintAllLexors() {
		
		}
		public void PrintLexor(Record rec) {
			try
			{
				Console.WriteLine(rec.token.PadRight(15) + rec.value);
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to PrintLexor: " + ex.Message);
			}
		}
		public void PrintHeader() {
			Console.WriteLine("\nIndentifier \t Value");
			Console.WriteLine("============================");
		}
		public void WriteLexor(string filename, Record rec) {
			try
			{
				using (StreamWriter sw = new StreamWriter(filename, true))
				{
					sw.WriteLine(rec.token.PadRight(15) + rec.value);
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to WriteLexor: " +ex.Message);
			}
		}
		public void WriteHeader(string filename) {
			try
			{
				File.Delete(filename);
				using (StreamWriter sw = new StreamWriter(filename, true)) {
					sw.WriteLine("\nIndentifier".PadRight(15) +  "Value");
					sw.WriteLine("============================");
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Error: Failed to WriteHeader: " + ex.Message);
			}
		}
		public void PrintCache() {
			foreach (char ch in cache) {
				Console.Write(ch);
			}
		}
		public string getString(char ch) {
			return ch.ToString();
		}
		public string TrimLeadingWhiteSpace(string word) {
			return word.Trim();
		}

	}
}
