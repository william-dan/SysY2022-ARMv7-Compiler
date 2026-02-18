%option noyywrap

%{
	#include <cstdlib>
	#include <string>
	#include "../structure/ast/ast.hpp"
	#include "parser.hpp"
%}


ID		[a-z_A-Z][0-9a-z_A-Z]*

FORMAT		[\"][^\n]*[\"]

INTNUM_OCT	0[0-7]+
INTNUM_HEX	0[xX][0-9a-fA-F]+
INTNUM_DEC	0|[1-9][0-9]*

FLOAT1	 ([0-9]*)\.[0-9]*
FLOAT2	 [0-9]+[eE][\+\-]?[0-9]+
FLOAT3	 [0-9]*\.[0-9]*[eE][\+\-]?[0-9]+
FLOAT_HEX 0[xX][0-9a-fA-F]*[\.]?[0-9a-fA-F]*[pP][\+\-]?[0-9]+

SKIP	 [ \n\t\r]

%start COMMENT
%start LCOMMENT

%%

<INITIAL>"//" {BEGIN LCOMMENT;}

<INITIAL>"/*" {BEGIN COMMENT;}

<LCOMMENT>\n {BEGIN INITIAL;}

<LCOMMENT>[^\n]* {}

<COMMENT>"*/" {BEGIN INITIAL;}

<COMMENT>[^\n\*]* {}

<COMMENT>\* {}

void 		{return VOID;}
int 		{return INT;}
float		{return FLOAT;}
const		{return CONST;}

if		{return IF;}
else 		{return ELSE;}
while		{return WHILE;}
break		{return BREAK;}
continue	{return CONTINUE;}
return		{return RETURN;}

starttime"("")"	{return STARTTIME;}
stoptime"("")"	{return STOPTIME;}


"+"  {return ADD;}
"-"  {return SUB;}
"!"  {return NOT;}
"*"  {yylval.mul_op = AST::mul_op::MULT; return MUL_OP;}
"/"  {yylval.mul_op = AST::mul_op::DIV; return MUL_OP;}
"%"  {yylval.mul_op = AST::mul_op::REM; return MUL_OP;}
"<"  {yylval.rel_op = AST::rel_op::LT; return REL_OP;}
">"  {yylval.rel_op = AST::rel_op::GT; return REL_OP;}
"<=" {yylval.rel_op = AST::rel_op::LE; return REL_OP;}
">=" {yylval.rel_op = AST::rel_op::GE; return REL_OP;}
"==" {yylval.equal_op = AST::equal_op::EQ; return EQUAL_OP;}
"!=" {yylval.equal_op = AST::equal_op::NE; return EQUAL_OP;}
"&&" {return LAND;}
"||" {return LOR;}


";"  {return SEMICOLON;}
"="  {return EQ;}
","  {return COMMA;}
"{"  {return LBRACE;}
"}"  {return RBRACE;}
"("  {return LPAREN;}
")"  {return RPAREN;}
"["  {return LBRACKET;}
"]"  {return RBRACKET;}


{FORMAT} {
	yylval.format = new std::string(yytext);
	return FORMAT;
}

{ID} {
	yylval.id = new std::string(yytext);
	return ID;
}

{INTNUM_OCT} {
	yylval.intnum = (int)strtol(yytext, (char **)NULL, 8);
	return INTNUM;
}

{INTNUM_HEX} {
	yylval.intnum = (int)strtol(yytext, (char **)NULL, 16);
	return INTNUM;
}

{INTNUM_DEC} {
	yylval.intnum = (int)strtol(yytext, (char **)NULL, 10);
	return INTNUM;
}

{FLOAT1} {
	yylval.floatnum = new std::string(yytext);
	return FLOATNUM;
}

{FLOAT2} {
	yylval.floatnum = new std::string(yytext);
	return FLOATNUM;
}

{FLOAT3} {
	yylval.floatnum = new std::string(yytext);
	return FLOATNUM;
}

{FLOAT_HEX} {
	yylval.floatnum = new std::string(yytext);
	return FLOATNUM;
}

{SKIP} {}


. {
	yylval.token = yytext[0];
	std::cerr << yytext << std::endl;
	exit(yytext[0]);
}


%%
