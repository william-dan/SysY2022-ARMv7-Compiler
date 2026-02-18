%{
	#include "../structure/ast/ast.hpp"
	
	#include <iostream>
	#include <cstdlib>
	#include <string>
	#include <cstdio>
	
	extern char *yytext;

	extern int yylex();
	extern int yyget_lineno();
	void yyerror(const char *s) { std::cerr << s << std::endl; exit(yytext[0]);}
	
	AST::CompUnits *root;

%}


%union {
	int token;
	
	std::string *id;
	int intnum;
	std::string *floatnum;
	std::string *format;
	
	AST::mul_op mul_op;
	AST::rel_op rel_op;
	AST::equal_op equal_op;
	
	AST::type btype;
	
	AST::CompUnits *root;
	AST::CompUnit *compunit;
	AST::Decl *decl;
	AST::ConstDecl *constdecl;
	AST::ConstDef *constdef;
	AST::ConstDefs *constdefs;
	AST::VarDecl *vardecl;
	AST::VarDefs *vardefs;
	AST::VarDef *vardef;
	AST::Arr_Index *arr_index;
	AST::Arr_Init *arr_init;
	AST::InitVals *initvals;
	AST::InitVal *initval;
	AST::FuncDef *funcdef;
	AST::Parameters *parameters;
	AST::Parameter *parameter;
	AST::Block *block;
	AST::BlockItems *blockitems;
	AST::BlockItem *blockitem;
	AST::Stmt *stmt;
	AST::Exp *exp;
	AST::IdExp *idexp;
	AST::Exps *exps;
	AST::Number *number;
	AST::Lval *lval;
	AST::unary_op unaryop;
}

%token<token> IF ELSE WHILE CONTINUE BREAK RETURN 
%token<token> VOID FLOAT CONST INT
%token<token> ADD SUB LAND LOR NOT
%token<token> LBRACE RBRACE
%token<token> LBRACKET RBRACKET
%token<token> LPAREN RPAREN
%token<token> SEMICOLON EQ COMMA
%token<token> STARTTIME STOPTIME

%token<mul_op> MUL_OP
%token<equal_op> EQUAL_OP
%token<rel_op> REL_OP
%token<intnum> INTNUM
%token<floatnum> FLOATNUM
%token<format> FORMAT
%token<id> ID

%type <root> COMPUNITS
%type <compunit> COMPUNIT
%type <decl> DECL
%type <constdecl> CONSTDECL
%type <vardecl> VARDECL
%type <constdefs> CONSTDEFS
%type <constdef> CONSTDEF
%type <vardefs> VARDEFS
%type <vardef> VARDEF
%type <arr_index> ARR_INDEX
%type <initval> INITVAL
%type <arr_init> ARR_INIT
%type <initvals> INITVALS
%type <block> BLOCK
%type <blockitems> BLOCKITEMS
%type <blockitem> BLOCKITEM
%type <funcdef> FUNCDEF
%type <parameters> PARAMETERS
%type <parameter> PARAMETER
%type <stmt> STMT
%type <stmt> IFSTMT WHILESTMT ASSIGNSTMT EXPSTMT BREAKSTMT CONTINUESTMT RETURNSTMT
%type <stmt> STARTTIMESTMT STOPTIMESTMT
%type <exps> EXPS
%type <exp> EXP
%type <exp> PRIMARYEXP UNARYEXP MULEXP ADDEXP RELEXP EQEXP LANDEXP LOREXP 
%type <exp> CALLEXP FORMATEXP
%type <idexp> IDEXP
%type <btype> BTYPE
%type <number> NUMBER
%type <unaryop> UNARYOP
%type <lval> LVAL

%left LOR
%left LAND
%left EQUAL_OP
%left REL_OP
%left ADD SUB
%left MUL_OP
%left prec1
%left prec2
%left ELSE

%start COMPUNITS

%%

COMPUNITS: COMPUNITS COMPUNIT {
	$$ = $1;
	$$->list.emplace_back($2);
	root = $$;
} 
| COMPUNIT {
	$$ = new AST::CompUnits(yyget_lineno());
	$$->list.emplace_back($1);
	root = $$;
}


COMPUNIT: DECL  {
	$$ = static_cast<AST::CompUnit*>($1);
} 
| FUNCDEF {
	$$ = static_cast<AST::CompUnit*>($1);
}


DECL: CONSTDECL {
	$$ = static_cast<AST::Decl*>($1);
} 
| VARDECL {
	$$ = static_cast<AST::Decl*>($1);
}


CONSTDECL: CONST BTYPE CONSTDEFS SEMICOLON {
	$$ = new AST::ConstDecl($2, $3, yyget_lineno());
}


CONSTDEFS: CONSTDEF {
	$$ = new AST::ConstDefs(yyget_lineno());
	$$->list.emplace_back($1);
} 
| CONSTDEFS COMMA CONSTDEF {
	$$ = $1;
	$1->list.emplace_back($3);
}


BTYPE: INT {
	$$ = AST::type::INT;
} 
| FLOAT {
	$$ = AST::type::FLOAT;
} 
| VOID {
	$$ = AST::type::VOID;
}


CONSTDEF: IDEXP ARR_INDEX EQ INITVAL {
	$$ = new AST::ConstDef($1, $2, $4, yyget_lineno());
}


VARDECL: BTYPE VARDEFS SEMICOLON {
	$$ = new AST::VarDecl($1, $2, yyget_lineno());
}


VARDEFS: VARDEF {
	$$ = new AST::VarDefs(yyget_lineno());
	$$->list.emplace_back($1);
} 
| VARDEFS COMMA VARDEF {
	$$ = $1;
	$$->list.emplace_back($3);
}


VARDEF: IDEXP ARR_INDEX {
	$$ = new AST::VarDef($1, $2, nullptr, yyget_lineno());
} 
| IDEXP ARR_INDEX EQ INITVAL {
	$$ = new AST::VarDef($1, $2, $4, yyget_lineno());
}


ARR_INDEX:  {
	$$ = new AST::Arr_Index(yyget_lineno());
} 
| ARR_INDEX LBRACKET EXP RBRACKET {
	$$ = $1;
	$$->list.emplace_back($3);
}


INITVAL: EXP {
	$$ = $1;
} 
| ARR_INIT {
	$$ = $1;
}


ARR_INIT: LBRACE INITVALS RBRACE {
	$$ = $2;
}


INITVALS: {
	$$ = new AST::InitVals(yyget_lineno());
} 
| INITVAL {
	$$ = new AST::InitVals($1, yyget_lineno());
} 
| INITVALS COMMA INITVAL {
	$$ = $1;
	$$->list.emplace_back($3);
}


FUNCDEF: BTYPE IDEXP LPAREN PARAMETERS RPAREN BLOCK {
	$$ = new AST::FuncDef($1, $2, $4, $6, yyget_lineno());
}


PARAMETERS:  {
	$$ = new AST::Parameters(yyget_lineno());
} 
| PARAMETER {
	$$ = new AST::Parameters($1, yyget_lineno());
} 
| PARAMETERS COMMA PARAMETER {
	$$ = $1;
	$$->list.emplace_back($3);
}


PARAMETER: BTYPE IDEXP {
	$$ = new AST::Parameter($1, $2, new AST::Arr_Index(yyget_lineno()), yyget_lineno());
} 
| BTYPE IDEXP LBRACKET RBRACKET ARR_INDEX {
	auto x = new AST::Arr_Index(yyget_lineno());
	x->list.emplace_back(new AST::IntNum(1, yyget_lineno()));
	for (const auto &y:$5->list) {
		x->list.emplace_back(y);
	}
	$$ = new AST::Parameter($1, $2, x, yyget_lineno());
}


BLOCK: LBRACE BLOCKITEMS RBRACE {
	$$ = new AST::Block($2, yyget_lineno());
}


BLOCKITEMS:  {
	$$ = new AST::BlockItems(yyget_lineno());
} | BLOCKITEMS BLOCKITEM {
	$$ = $1;
	$$->list.emplace_back($2);
}


BLOCKITEM: DECL {
	$$ = $1;
} | STMT {
	$$ = $1;
}


STMT: ASSIGNSTMT SEMICOLON {
	$$ = $1;
} 
| SEMICOLON {
	$$ = new AST::ExpStmt(new AST::IntNum(0, yyget_lineno()), yyget_lineno());
} 
| EXPSTMT SEMICOLON {
	$$ = $1;
} 
| BLOCK {
	$$ = $1;
} 
| IFSTMT {
	$$ = $1;
} 
| WHILESTMT {
	$$ = $1;
} 
| BREAKSTMT SEMICOLON {
	$$ = $1;
} 
| CONTINUESTMT SEMICOLON {
	$$ = $1;
} 
| RETURNSTMT SEMICOLON {
	$$ = $1;
} 
| STARTTIMESTMT SEMICOLON {
	$$ = $1;
} 
| STOPTIMESTMT SEMICOLON {
	$$ = $1;
}


ASSIGNSTMT: LVAL EQ EXP {
	$$ = new AST::AssignStmt($1, $3, yyget_lineno());
}


EXPSTMT: EXP {
	$$ = new AST::ExpStmt($1, yyget_lineno());
}


IFSTMT: IF LPAREN EXP RPAREN STMT %prec prec2 {
	$$ = new AST::IfStmt($3, $5, nullptr, yyget_lineno());
} 
| IF LPAREN EXP RPAREN STMT ELSE STMT {
	$$ = new AST::IfStmt($3, $5, $7, yyget_lineno());
}


WHILESTMT: WHILE LPAREN EXP RPAREN STMT {
	$$ = new AST::WhileStmt($3, $5, yyget_lineno());
}


BREAKSTMT: BREAK {
	$$ = new AST::BreakStmt(yyget_lineno());
}


CONTINUESTMT: CONTINUE {
	$$ = new AST::ContinueStmt(yyget_lineno());
}


RETURNSTMT: RETURN {
	$$ = new AST::ReturnStmt(nullptr, yyget_lineno());
} 
| RETURN EXP {
	$$ = new AST::ReturnStmt($2, yyget_lineno());
}


STARTTIMESTMT: STARTTIME {
	$$ = new AST::StarttimeStmt(yyget_lineno());
}


STOPTIMESTMT: STOPTIME {
	$$ = new AST::StoptimeStmt(yyget_lineno());
}


EXP: PRIMARYEXP {
	$$ = $1;
} 
| UNARYEXP {
	$$ = $1;
} 
| MULEXP {
	$$ = $1;
} 
| ADDEXP {
	$$ = $1;
} 
| RELEXP {
	$$ = $1;
} 
| EQEXP {
	$$ = $1;
} 
| LANDEXP {
	$$ = $1;
} 
| LOREXP {
	$$ = $1;
} 
| CALLEXP {
	$$ = $1;
} 
| FORMATEXP {
	$$ = $1;
}


IDEXP: ID {
	$$ = new AST::IdExp($1, yyget_lineno());
}


LVAL: IDEXP ARR_INDEX {
	$$ = new AST::Lval($1, $2, yyget_lineno());
}


PRIMARYEXP: LPAREN EXP RPAREN {
	$$ = $2;
} 
| LVAL {
	$$ = $1;
} 
| NUMBER {
	$$ = $1;
}


NUMBER: INTNUM {
	$$ = new AST::IntNum($1, yyget_lineno());
} 
| FLOATNUM {
	$$ = new AST::FloatNum($1, yyget_lineno());
}


UNARYEXP: UNARYOP EXP %prec prec1 {
	$$ = new AST::UnaryExp($1, $2, yyget_lineno());
} 


CALLEXP: IDEXP LPAREN EXPS RPAREN {
	$$ = new AST::CallExp($1, $3, yyget_lineno());
}


UNARYOP: ADD {
	$$ = AST::unary_op::ADD;
} 
| SUB {
	$$ = AST::unary_op::SUB;
} 
| NOT {
	$$ = AST::unary_op::NOT;
}


EXPS:  {
	$$ = new AST::Exps(yyget_lineno());
} 
| EXP {
	$$ = new AST::Exps($1, yyget_lineno());
} 
| EXPS COMMA EXP {
	$$ = $1;
	$$->list.emplace_back($3);
}


MULEXP: EXP MUL_OP EXP {
	$$ = new AST::MulExp($1, $2, $3, yyget_lineno());
}


ADDEXP: EXP ADD EXP {
	$$ = new AST::AddExp($1, AST::add_op::ADD, $3, yyget_lineno());
} 
| EXP SUB EXP {
	$$ = new AST::AddExp($1, AST::add_op::SUB, $3, yyget_lineno());
}


RELEXP: EXP REL_OP EXP {
	$$ = new AST::RelExp($1, $2, $3, yyget_lineno());
}


EQEXP: EXP EQUAL_OP EXP {
	$$ = new AST::EqExp($1, $2, $3, yyget_lineno());
}


LANDEXP: EXP LAND EXP {
	$$ = new AST::LAndExp($1, $3, yyget_lineno());
}


LOREXP: EXP LOR EXP {
	$$ = new AST::LOrExp($1, $3, yyget_lineno());
}


FORMATEXP: FORMAT {
	$$ = new AST::FormatExp($1, yyget_lineno());
}

%%
