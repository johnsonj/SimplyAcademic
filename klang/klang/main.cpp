//
//  klang
//  based off of: http://llvm.org/docs/tutorial/
//


#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

//
// Lexer
//

enum class Token : int
{
    t_eof = -1,
    t_def = -2,
    t_extern = -3,
    t_identifier = -4,
    t_number = -5,
};

static std::string g_identifierName;
static double g_numberVal;
    
static int gettoken()
{
    static int lastChar = static_cast<int>(' ');
    
    while(isspace(lastChar))
        lastChar = getchar();
    
    if(isalpha(lastChar))
    {
        g_identifierName = lastChar;
        while(isalnum(lastChar = getchar()))
            g_identifierName += lastChar;
        
        if(g_identifierName == "def")
            return static_cast<int>(Token::t_def);
        if(g_identifierName == "extern")
            return static_cast<int>(Token::t_extern);

        return static_cast<int>(Token::t_identifier);
    }
    
    if(isdigit(lastChar))
    {
        std::string numStr;
    
        do
        {
            numStr += lastChar;
            lastChar = getchar();
        } while(isdigit(lastChar) || lastChar == '.');
        
        g_numberVal = strtod(numStr.c_str(), nullptr);
        
        return static_cast<int>(Token::t_number);
    }
    
    if(lastChar == '#')
    {
        do
        {
            lastChar = getchar();
        } while(lastChar != EOF && lastChar != '\n' && lastChar != '\r');
        
        if(lastChar != EOF)
            return gettoken();
    }
    
    if(lastChar == EOF)
        return static_cast<int>(Token::t_eof);

    int thisChar = lastChar;
    lastChar = getchar();
    
    return thisChar;
}

//
// AST Definitions
//

class ExprAst
{
public:
    virtual ~ExprAst() {}
};
    
class NumberExprAst : public ExprAst
{
    double val;

public:
    NumberExprAst(double _val) : val(_val) {}
};
    
class VariableExprAst : public ExprAst
{
    std::string name;

public:
    VariableExprAst(const std::string& _name) : name(_name) {}
};
    
class BinaryExprAst : public ExprAst
{
    char op;
    std::unique_ptr<ExprAst> lhs, rhs;

public:
    BinaryExprAst(char _op,
                  std::unique_ptr<ExprAst> _lhs,
                  std::unique_ptr<ExprAst> _rhs) :
    op(_op), lhs(std::move(_lhs)), rhs(std::move(_rhs)) {}
};

class CallExprAst : public ExprAst
{
    std::string callee;
    std::vector<std::unique_ptr<ExprAst>> args;

public:
    CallExprAst(const std::string& _callee,
                std::vector<std::unique_ptr<ExprAst>> _args) :
    callee(_callee), args(std::move(_args)) {}
};

class PrototypeAst
{
    std::string name;
    std::vector<std::string> args;

public:
    PrototypeAst(const std::string& _name,
                std::vector<std::string> _args) :
    name(_name), args(std::move(_args)) {}
};

class FunctionAst
{
    std::unique_ptr<PrototypeAst> proto;
    std::unique_ptr<ExprAst> body;

public:
    FunctionAst(std::unique_ptr<PrototypeAst> _proto,
                std::unique_ptr<ExprAst> _body) :
    proto(std::move(_proto)), body(std::move(_body)) {}
};

//
// AST Helpers
//
static int g_curToken = -1;
static int GetNextToken()
{
    return g_curToken = gettoken();
}

std::unique_ptr<ExprAst> Error(const char* szStr)
{
    std::cout << "Error: " << szStr << "\n";
    return nullptr;
}

std::unique_ptr<PrototypeAst> ErrorP(const char* szStr)
{
    std::cout << "Error: " << szStr << "\n";
    return nullptr;
}

//
// AST Builders
//
std::unique_ptr<ExprAst> ParseExpression();
std::unique_ptr<ExprAst> ParseBinOpRHS(int exprPresc, std::unique_ptr<ExprAst> lhs);

std::unique_ptr<ExprAst> ParseNumberExpr()
{
    auto expr = std::make_unique<NumberExprAst>(g_numberVal);
    (void)GetNextToken();
    return std::move(expr);
}

//  parrenexpr ::= identifier '(' expression ')'
std::unique_ptr<ExprAst> ParseParenExpr()
{
    (void)GetNextToken(); // eat '('
    auto expr = ParseExpression();
    if(!expr)
        return nullptr;
    
    if(g_curToken != ')')
        return Error("expected ')'");
    
    (void)GetNextToken(); // eat ')'
    return expr;
}

//  identifierexpr
//      ::= identifier
//      ::= identifier '(' expression* ')'
std::unique_ptr<ExprAst> ParseIdentifierExpr()
{
    std::string identifierName = g_identifierName;

    (void)GetNextToken(); // eat identifierName
    
    if(g_curToken != '(')
        return std::make_unique<VariableExprAst>(identifierName);
    
    // Function call
    (void)GetNextToken(); // eat '('
    std::vector<std::unique_ptr<ExprAst>> args;
    // Collect args
    while(true)
    {
        if(auto arg = ParseExpression())
            args.push_back(std::move(arg));
        else
            return nullptr; // TODO: Should this be an error?
        
        if(g_curToken == ')')
            break; // all done!
        
        if(g_curToken != ',')
            return Error("Expected ')' or ',' in arguments list");

        (void)GetNextToken(); // advance
    }
    
    (void)GetNextToken(); // eat ')'
    
    return std::make_unique<CallExprAst>(identifierName, std::move(args));
}

std::unique_ptr<ExprAst> ParsePrimary()
{
    switch(g_curToken)
    {
    default:
        return Error("unkown token when expecting an expression");
    case static_cast<int>(Token::t_identifier):
        return ParseIdentifierExpr();
    case static_cast<int>(Token::t_number):
        return ParseNumberExpr();
    case '(':
        return ParseParenExpr();
    }
}

int GetTokenPrecedence()
{
    switch(g_curToken)
    {
        case '<':
            return 10;
        case '+':
            return 20;
        case '-':
            return 30;
        case '*':
            return 40;
        default:
            return -1;
    }
}

std::unique_ptr<ExprAst> ParseExpression()
{
    auto lhs = ParsePrimary();
    if(!lhs)
        return nullptr;
    
    return ParseBinOpRHS(0, std::move(lhs));
}


//  binopsrhs
//      ::= ('+' primary)*
std::unique_ptr<ExprAst> ParseBinOpRHS(int exprPresc, std::unique_ptr<ExprAst> lhs)
{
    while(true)
    {
        int tokenPresc = GetTokenPrecedence();
        
        // If the binop has a lower precedence than the current (passed in) binop
        // consome it, otherwise we're done
        if(tokenPresc < exprPresc)
            return lhs;
    
        int binop = g_curToken;
        (void)GetNextToken(); // swallow binop
        
        auto rhs = ParsePrimary();
        if(!rhs)
            return nullptr;
        
        // If the binop binds less tightly with RHS than the operator after RHS,
        // let the pending operator take RHS as its LHS
        int nextTokenPresc = GetTokenPrecedence();
        if(tokenPresc < nextTokenPresc)
        {
            rhs = ParseBinOpRHS(tokenPresc + 1, std::move(rhs));
            if(!rhs)
                return nullptr;
        }
        
        // Merge LHS & RHS
        lhs = std::make_unique<BinaryExprAst>(binop, std::move(lhs), std::move(rhs));
    }
}

//  prototype
//      ::= id '(' id ')'
std::unique_ptr<PrototypeAst> ParsePrototype()
{
    if(g_curToken != static_cast<int>(Token::t_identifier))
        return ErrorP("expected function name");
    
    std::string fnName = g_identifierName;
    (void)GetNextToken(); // swallow name
    
    if(g_curToken != '(')
        return ErrorP("expected '(' in prototype");
    
    std::vector<std::string> args;
    while(GetNextToken() == static_cast<int>(Token::t_identifier))
        args.push_back(g_identifierName);
    
    if(g_curToken != ')')
        return ErrorP("expected ')' in prototype");
    
    (void)GetNextToken(); // swallow ')'
    
    return std::make_unique<PrototypeAst>(fnName, std::move(args));
}

// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAst> ParseDefinition()
{
    (void)GetNextToken(); // eat 'def'
    auto proto = ParsePrototype();
    if(!proto)
        return nullptr;
    
    if(auto expr = ParseExpression())
        return std::make_unique<FunctionAst>(std::move(proto), std::move(expr));
    
    return nullptr;
}

// external ::= 'extern' prototype
std::unique_ptr<PrototypeAst> ParseExtern()
{
    (void)GetNextToken(); // eat 'extern'
    return ParsePrototype();
}

// topLevelExpr ::= expression
std::unique_ptr<FunctionAst> ParseTopLevelExpr()
{
    if(auto expr = ParseExpression())
    {
        auto proto = std::make_unique<PrototypeAst>("", std::vector<std::string>());
        return std::make_unique<FunctionAst>(std::move(proto), std::move(expr));
    }
    return nullptr;
}


// Parser Driver
void HandleDefinition()
{
    if(ParseDefinition())
        std::cout << "Parsed a function definition" << std::endl;
    else
        (void)GetNextToken();
}

void HandleExtern()
{
    if(ParseExtern())
        std::cout << "Parsed an extern" << std::endl;
    else
        (void)GetNextToken();
}
void HandleTopLevelExpression()
{
    if(ParseTopLevelExpr())
        std::cout << "Parsed a top level expression" << std::endl;
    else
        (void)GetNextToken();
}

void MainParserLoop()
{
    while(true)
    {
        std::cout << "ready> ";
        switch(g_curToken)
        {
            case static_cast<int>(Token::t_eof):
                return;
            case static_cast<int>(Token::t_def):
                HandleDefinition();
                break;
            case static_cast<int>(Token::t_extern):
                HandleExtern();
                break;
            case ';':
                (void)GetNextToken(); // ignore top level semicolon
                break;
            default:
                HandleTopLevelExpression();
                break;
        }
    }
}

int main(int argc, const char * argv[]) {
    // prime first token
    std::cout << "ready> ";
    (void)GetNextToken();
    
    MainParserLoop();
    return 0;
}
