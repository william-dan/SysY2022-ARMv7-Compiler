/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_HPP_INCLUDED
# define YY_YY_PARSER_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    IF = 258,                      /* IF  */
    ELSE = 259,                    /* ELSE  */
    WHILE = 260,                   /* WHILE  */
    CONTINUE = 261,                /* CONTINUE  */
    BREAK = 262,                   /* BREAK  */
    RETURN = 263,                  /* RETURN  */
    VOID = 264,                    /* VOID  */
    FLOAT = 265,                   /* FLOAT  */
    CONST = 266,                   /* CONST  */
    INT = 267,                     /* INT  */
    ADD = 268,                     /* ADD  */
    SUB = 269,                     /* SUB  */
    LAND = 270,                    /* LAND  */
    LOR = 271,                     /* LOR  */
    NOT = 272,                     /* NOT  */
    LBRACE = 273,                  /* LBRACE  */
    RBRACE = 274,                  /* RBRACE  */
    LBRACKET = 275,                /* LBRACKET  */
    RBRACKET = 276,                /* RBRACKET  */
    LPAREN = 277,                  /* LPAREN  */
    RPAREN = 278,                  /* RPAREN  */
    SEMICOLON = 279,               /* SEMICOLON  */
    EQ = 280,                      /* EQ  */
    COMMA = 281,                   /* COMMA  */
    STARTTIME = 282,               /* STARTTIME  */
    STOPTIME = 283,                /* STOPTIME  */
    MUL_OP = 284,                  /* MUL_OP  */
    EQUAL_OP = 285,                /* EQUAL_OP  */
    REL_OP = 286,                  /* REL_OP  */
    INTNUM = 287,                  /* INTNUM  */
    FLOATNUM = 288,                /* FLOATNUM  */
    FORMAT = 289,                  /* FORMAT  */
    ID = 290,                      /* ID  */
    prec1 = 291,                   /* prec1  */
    prec2 = 292                    /* prec2  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 20 "parser.yy"

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

#line 143 "parser.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_HPP_INCLUDED  */
