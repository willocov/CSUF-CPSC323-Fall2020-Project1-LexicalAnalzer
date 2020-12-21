using System;
using System.Linq;

namespace LexicalAnalyzer
{
    class Program
    {
        static void Main(string[] args)
        {
            int argCount = args.Count();

            //string inputFile = "input.txt", outputFile = "outputFile.txt";

            if (argCount < 2)
            {
                Console.WriteLine("Error: Not Enough Arguments");
                return;
            }

            string inputFile = null, outputFile = null;
            try
            {
                inputFile = args[0];
                outputFile = args[1];
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: Failed to get arguements: " + ex.Message);
                return;
            }

            LexicalAnalyzer lex = new LexicalAnalyzer();
            try
            {
                lex.LoadFile(inputFile);    //Loads text from file into char cache
                lex.ParseRecords();         //Parses all Lexors out of input file and saves them to a vector of Records

                //Exit early if no lexors are parsed
                if (lex.records.Count == 0)
                {
                    Console.WriteLine("No Lexors Found In Records");
                    return;
                }

                //Loop through Records and print/write Lexors
                lex.PrintHeader();
                lex.WriteHeader(outputFile);
                foreach (Record rec in lex.records) {
                    lex.PrintLexor(rec);
                    lex.WriteLexor(outputFile, rec);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("Error: Lexical Analyzer Failed: " + ex.Message);
            }
        }
    }
}
