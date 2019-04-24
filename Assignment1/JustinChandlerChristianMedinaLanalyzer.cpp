//Justin Chandler
//Christian Medina
#include<fstream>
#include<iostream>
#include<string>
#include<vector>
#include<stdlib.h>

using namespace std;

//Lexical Analyzer starts here

//Our Finite State Machine is limited by the fact that it must have integer, real, and identifier
enum possibleTokens
{
 END = 0,
 INTEGER,
 REAL,
 IDENTIFIER,
 OPERATOR,
 WHITESPACE,
 SEPARATOR,

};

int fstateMachine[][7] = {
 { END,         INTEGER,      REAL,         IDENTIFIER,   OPERATOR, WHITESPACE,  SEPARATOR  },
 { INTEGER,     INTEGER,      REAL,         END,          END,      END,        END         },
 { REAL,        REAL,         END,          END,          END,      END,        END         },
 { IDENTIFIER,  IDENTIFIER,   IDENTIFIER,   IDENTIFIER,   END,      END,        END         },
 { OPERATOR,    END,          END,          END,          OPERATOR, END,        END         },
 { WHITESPACE,  END,          END,          END,          END,      END,        END         },
 { SEPARATOR,   END,          END,          END,          END,      END,        END         }
};

struct TokenProperties  //Keeps track of token information
{
 string lexemeLabel;
 string token;
 int lexemeInt;
 int lineNumber;
};

void removeComments(string input)
{
  string line;
  ifstream MyFileStream(input.c_str());
  bool skip = false;
  ofstream MyCommentless("commentless.txt");
  while (getline(MyFileStream, line))
  {
    for(int i = 0; i < line.length(); i++)
    {
      if (line[i] == '[' && line[i+1] == '*')
      {
        skip = true;
      }

      if (skip == false)
      {
        MyCommentless << line[i];
      }

      if (line[i] == '*' && line[i+1] == ']')
      {
        skip = false;
        i++;
      }

    }
  }
}

void checkKeyword(TokenProperties &in) //Simple if statement to check if this identifier is actually a keyword
{
 if (in.token == "while" || in.token == "whileend" || in.token == "function" || in.token == "if" || in.token == "ifend" || in.token == "put" || in.token == "else" || in.token == "int" || in.token == "return" || in.token == "get" || in.token == "float")
  in.lexemeLabel = "KEYWORD";
}

int checkState(char in)  //Determine the state based on what character is being input
{
 int det = 0;

 if (in == ' ' || in == ' ')
  det = WHITESPACE;

 else if (isdigit(in))
  det = INTEGER;

 else if (in == '.')
  det = REAL;

 else if (isalpha(in))
  det = IDENTIFIER;

 else if (in == ':' || in == '=' || in == '<' || in == '>' || in == '-' || in == '+' || in == '*' || in == '/' || in == '%' || in == '!')
  det = OPERATOR;

 else if (in == '(' || in == ')' || in == '{' || in == '}' || in == ';' || in == ',' || in == '[' || in == ']' || in == '$')
  det = SEPARATOR;

 return det;
}

string stateToString(int lexemeInt) //Convert the lexeme integer to its proper string
{
 string asText = "WHITESPACE";

 if (lexemeInt == INTEGER)
  asText = "INTEGER";

 else if (lexemeInt == REAL)
  asText = "REAL";

 else if (lexemeInt == IDENTIFIER)
  asText = "IDENTIFIER";

 else if (lexemeInt == OPERATOR)
  asText = "OPERATOR";

 else if (lexemeInt == SEPARATOR)
  asText = "SEPARATOR";

 return asText;
}

vector<TokenProperties> Lexer(string inputScript, const int lineNumber) //Main lexer function
{
 TokenProperties MyToken;
 vector<TokenProperties> result;

 char curChar;

 int loc = END;
 int curState = END;
 int prevState = END;

 string curToken;

 for (int i = 0; i < inputScript.length(); i++)
 {
  curChar = inputScript[i];
  loc = checkState(curChar); //Determine the location on FSM

  curState = fstateMachine[curState][loc]; //Update to current state

  if (curState == END) //Have we reached an end marker?
  {
   if (prevState != WHITESPACE) //Have we actually read anything?
   {
    MyToken.token = curToken;
    MyToken.lexemeInt = prevState;
    MyToken.lexemeLabel = stateToString(MyToken.lexemeInt);
    MyToken.lineNumber = lineNumber;

    int lastChar = curToken.length() - 1;

    if (MyToken.lexemeLabel == "IDENTIFIER" && isdigit(curToken[lastChar])) //If a variable ends in a digit
    {
      MyToken.lexemeLabel = "INVALID";
      cout << "\nAn invalid identifier was encountered! Check your code!\nIdentifier: " << MyToken.token << " ends in an integer! \n";
    }

    checkKeyword(MyToken); //Check if token is a keyword
    result.push_back(MyToken);
   }
   curToken = ""; //Reset curToken
   i--;
  }

  else
  {
   curToken += curChar;
  }
  prevState = curState;
 }

 if (curState != WHITESPACE) {
  MyToken.token = curToken;
  MyToken.lexemeInt = curState;
  MyToken.lexemeLabel = stateToString(MyToken.lexemeInt);
  MyToken.lineNumber = lineNumber;

  checkKeyword(MyToken); //Check if token is a keyword
  result.push_back(MyToken);
 }

 return result;
}

//End of Lexical Analyzer definitions

int main()
{
 string line, MyInput;

 cout << "Please enter the name of your input file: \n";

 cin >> MyInput;

 vector <TokenProperties> MyTokens;

 ifstream MyFileStream(MyInput.c_str()); //Convert input to a C string and then a filestream
 ofstream MyOutput("result.txt");

 if (MyFileStream.fail())
 {
  cout << "\n** ERROR file not found. Please make sure you entered the filename correctly, ending with .txt\n";
  system("pause");
  return 0;
 }

 else
 {
  cout << "File loaded...\n";
 }

 int CurrentLineNumber = 0;
 vector<TokenProperties> allTokens;

 removeComments(MyInput);

 ifstream MyCommentlessStream("commentless.txt");
 cout << "\nComments removed.\n";

 while (getline(MyCommentlessStream, line))
 {
  CurrentLineNumber++;
  MyTokens = Lexer(line, CurrentLineNumber);

  for (int i = 0; i < MyTokens.size(); i++)
  {
   allTokens.push_back(MyTokens[i]);
   if (MyTokens[i].lexemeLabel == "INTEGER" || MyTokens[i].lexemeLabel == "KEYWORD")
   {
     MyOutput << MyTokens[i].lexemeLabel << "\t\t\t\t" << MyTokens[i].token << endl;
   }
   else
   {
     MyOutput << MyTokens[i].lexemeLabel << "\t\t\t" << MyTokens[i].token << endl;
   }
  }
 }

 //Lexical Analyzer fully executed at this point

 cout << "Lexical Analyzer complete. Exported to result.txt\n\n";
 system("pause");
 return 0;
}
