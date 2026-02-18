/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "parser.yy"

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


#line 89 "parser.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IF = 3,                         /* IF  */
  YYSYMBOL_ELSE = 4,                       /* ELSE  */
  YYSYMBOL_WHILE = 5,                      /* WHILE  */
  YYSYMBOL_CONTINUE = 6,                   /* CONTINUE  */
  YYSYMBOL_BREAK = 7,                      /* BREAK  */
  YYSYMBOL_RETURN = 8,                     /* RETURN  */
  YYSYMBOL_VOID = 9,                       /* VOID  */
  YYSYMBOL_FLOAT = 10,                     /* FLOAT  */
  YYSYMBOL_CONST = 11,                     /* CONST  */
  YYSYMBOL_INT = 12,                       /* INT  */
  YYSYMBOL_ADD = 13,                       /* ADD  */
  YYSYMBOL_SUB = 14,                       /* SUB  */
  YYSYMBOL_LAND = 15,                      /* LAND  */
  YYSYMBOL_LOR = 16,                       /* LOR  */
  YYSYMBOL_NOT = 17,                       /* NOT  */
  YYSYMBOL_LBRACE = 18,                    /* LBRACE  */
  YYSYMBOL_RBRACE = 19,                    /* RBRACE  */
  YYSYMBOL_LBRACKET = 20,                  /* LBRACKET  */
  YYSYMBOL_RBRACKET = 21,                  /* RBRACKET  */
  YYSYMBOL_LPAREN = 22,                    /* LPAREN  */
  YYSYMBOL_RPAREN = 23,                    /* RPAREN  */
  YYSYMBOL_SEMICOLON = 24,                 /* SEMICOLON  */
  YYSYMBOL_EQ = 25,                        /* EQ  */
  YYSYMBOL_COMMA = 26,                     /* COMMA  */
  YYSYMBOL_STARTTIME = 27,                 /* STARTTIME  */
  YYSYMBOL_STOPTIME = 28,                  /* STOPTIME  */
  YYSYMBOL_MUL_OP = 29,                    /* MUL_OP  */
  YYSYMBOL_EQUAL_OP = 30,                  /* EQUAL_OP  */
  YYSYMBOL_REL_OP = 31,                    /* REL_OP  */
  YYSYMBOL_INTNUM = 32,                    /* INTNUM  */
  YYSYMBOL_FLOATNUM = 33,                  /* FLOATNUM  */
  YYSYMBOL_FORMAT = 34,                    /* FORMAT  */
  YYSYMBOL_ID = 35,                        /* ID  */
  YYSYMBOL_prec1 = 36,                     /* prec1  */
  YYSYMBOL_prec2 = 37,                     /* prec2  */
  YYSYMBOL_YYACCEPT = 38,                  /* $accept  */
  YYSYMBOL_COMPUNITS = 39,                 /* COMPUNITS  */
  YYSYMBOL_COMPUNIT = 40,                  /* COMPUNIT  */
  YYSYMBOL_DECL = 41,                      /* DECL  */
  YYSYMBOL_CONSTDECL = 42,                 /* CONSTDECL  */
  YYSYMBOL_CONSTDEFS = 43,                 /* CONSTDEFS  */
  YYSYMBOL_BTYPE = 44,                     /* BTYPE  */
  YYSYMBOL_CONSTDEF = 45,                  /* CONSTDEF  */
  YYSYMBOL_VARDECL = 46,                   /* VARDECL  */
  YYSYMBOL_VARDEFS = 47,                   /* VARDEFS  */
  YYSYMBOL_VARDEF = 48,                    /* VARDEF  */
  YYSYMBOL_ARR_INDEX = 49,                 /* ARR_INDEX  */
  YYSYMBOL_INITVAL = 50,                   /* INITVAL  */
  YYSYMBOL_ARR_INIT = 51,                  /* ARR_INIT  */
  YYSYMBOL_INITVALS = 52,                  /* INITVALS  */
  YYSYMBOL_FUNCDEF = 53,                   /* FUNCDEF  */
  YYSYMBOL_PARAMETERS = 54,                /* PARAMETERS  */
  YYSYMBOL_PARAMETER = 55,                 /* PARAMETER  */
  YYSYMBOL_BLOCK = 56,                     /* BLOCK  */
  YYSYMBOL_BLOCKITEMS = 57,                /* BLOCKITEMS  */
  YYSYMBOL_BLOCKITEM = 58,                 /* BLOCKITEM  */
  YYSYMBOL_STMT = 59,                      /* STMT  */
  YYSYMBOL_ASSIGNSTMT = 60,                /* ASSIGNSTMT  */
  YYSYMBOL_EXPSTMT = 61,                   /* EXPSTMT  */
  YYSYMBOL_IFSTMT = 62,                    /* IFSTMT  */
  YYSYMBOL_WHILESTMT = 63,                 /* WHILESTMT  */
  YYSYMBOL_BREAKSTMT = 64,                 /* BREAKSTMT  */
  YYSYMBOL_CONTINUESTMT = 65,              /* CONTINUESTMT  */
  YYSYMBOL_RETURNSTMT = 66,                /* RETURNSTMT  */
  YYSYMBOL_STARTTIMESTMT = 67,             /* STARTTIMESTMT  */
  YYSYMBOL_STOPTIMESTMT = 68,              /* STOPTIMESTMT  */
  YYSYMBOL_EXP = 69,                       /* EXP  */
  YYSYMBOL_IDEXP = 70,                     /* IDEXP  */
  YYSYMBOL_LVAL = 71,                      /* LVAL  */
  YYSYMBOL_PRIMARYEXP = 72,                /* PRIMARYEXP  */
  YYSYMBOL_NUMBER = 73,                    /* NUMBER  */
  YYSYMBOL_UNARYEXP = 74,                  /* UNARYEXP  */
  YYSYMBOL_CALLEXP = 75,                   /* CALLEXP  */
  YYSYMBOL_UNARYOP = 76,                   /* UNARYOP  */
  YYSYMBOL_EXPS = 77,                      /* EXPS  */
  YYSYMBOL_MULEXP = 78,                    /* MULEXP  */
  YYSYMBOL_ADDEXP = 79,                    /* ADDEXP  */
  YYSYMBOL_RELEXP = 80,                    /* RELEXP  */
  YYSYMBOL_EQEXP = 81,                     /* EQEXP  */
  YYSYMBOL_LANDEXP = 82,                   /* LANDEXP  */
  YYSYMBOL_LOREXP = 83,                    /* LOREXP  */
  YYSYMBOL_FORMATEXP = 84                  /* FORMATEXP  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  13
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   233

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  47
/* YYNRULES -- Number of rules.  */
#define YYNRULES  93
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  150

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   292


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   125,   125,   130,   137,   140,   145,   148,   153,   158,
     162,   168,   171,   174,   179,   184,   189,   193,   199,   202,
     207,   210,   216,   219,   224,   229,   232,   235,   241,   246,
     249,   252,   258,   261,   271,   276,   278,   284,   286,   291,
     294,   297,   300,   303,   306,   309,   312,   315,   318,   321,
     326,   331,   336,   339,   344,   349,   354,   359,   362,   367,
     372,   377,   380,   383,   386,   389,   392,   395,   398,   401,
     404,   409,   414,   419,   422,   425,   430,   433,   438,   443,
     448,   451,   454,   459,   462,   465,   471,   476,   479,   484,
     489,   494,   499,   504
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IF", "ELSE", "WHILE",
  "CONTINUE", "BREAK", "RETURN", "VOID", "FLOAT", "CONST", "INT", "ADD",
  "SUB", "LAND", "LOR", "NOT", "LBRACE", "RBRACE", "LBRACKET", "RBRACKET",
  "LPAREN", "RPAREN", "SEMICOLON", "EQ", "COMMA", "STARTTIME", "STOPTIME",
  "MUL_OP", "EQUAL_OP", "REL_OP", "INTNUM", "FLOATNUM", "FORMAT", "ID",
  "prec1", "prec2", "$accept", "COMPUNITS", "COMPUNIT", "DECL",
  "CONSTDECL", "CONSTDEFS", "BTYPE", "CONSTDEF", "VARDECL", "VARDEFS",
  "VARDEF", "ARR_INDEX", "INITVAL", "ARR_INIT", "INITVALS", "FUNCDEF",
  "PARAMETERS", "PARAMETER", "BLOCK", "BLOCKITEMS", "BLOCKITEM", "STMT",
  "ASSIGNSTMT", "EXPSTMT", "IFSTMT", "WHILESTMT", "BREAKSTMT",
  "CONTINUESTMT", "RETURNSTMT", "STARTTIMESTMT", "STOPTIMESTMT", "EXP",
  "IDEXP", "LVAL", "PRIMARYEXP", "NUMBER", "UNARYEXP", "CALLEXP",
  "UNARYOP", "EXPS", "MULEXP", "ADDEXP", "RELEXP", "EQEXP", "LANDEXP",
  "LOREXP", "FORMATEXP", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-85)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      97,   -85,   -85,    56,   -85,   221,   -85,   -85,   -85,    -7,
     -85,   -85,    -7,   -85,   -85,   -85,    -6,   -85,    10,    68,
     -85,   -85,   -85,    -7,    56,    -9,   -85,    -7,    -3,   -85,
     -85,    -7,    60,   -85,    65,    41,   -85,    41,    18,    30,
      56,   -85,   -85,   -85,    65,   -85,   -85,   -85,   166,    27,
     -85,   -85,   -85,   -85,   -85,    65,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,    41,   -85,   -85,    21,   -85,    49,   -85,
     -85,   -85,   170,    65,    65,    65,    65,   -85,    65,    65,
      65,    65,    42,   -85,   -85,   -14,   -85,   110,   -85,    48,
      48,    -5,   198,   -85,     0,    43,    21,    62,   -85,    41,
      42,    67,    69,   -85,   -85,    65,   -85,   -85,   -85,   -85,
     -85,    -7,   -85,   -85,   -85,    77,    78,   -85,   -85,    80,
      88,   101,   102,   106,    21,   108,   -85,    65,   -85,    65,
      65,    21,   -85,   -85,   -85,   -85,   -85,   -85,   -85,    65,
      21,   175,   194,    21,   143,   143,   127,   -85,   143,   -85
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    13,    12,     0,    11,     0,     3,     4,     6,     0,
       7,     5,     0,     1,     2,    71,     0,    16,    20,     0,
       9,    20,    15,     0,    29,    18,     8,     0,     0,    17,
      20,     0,     0,    30,     0,     0,    10,     0,    32,     0,
       0,    80,    81,    82,     0,    76,    77,    93,     0,    20,
      74,    61,    75,    62,    69,     0,    63,    64,    65,    66,
      67,    68,    70,    25,    19,    23,    22,    14,     0,    35,
      28,    31,     0,     0,     0,     0,     0,    21,     0,     0,
       0,    83,    72,    78,    26,     0,    20,     0,    73,    87,
      88,    91,    92,    86,    90,    89,    84,     0,    24,     0,
      33,     0,     0,    56,    55,    57,    34,    40,    59,    60,
      37,     0,    42,    36,    38,     0,     0,    43,    44,     0,
       0,     0,     0,     0,    51,    74,    79,     0,    27,     0,
       0,    58,    39,    41,    45,    46,    47,    48,    49,     0,
      85,     0,     0,    50,     0,     0,    52,    54,     0,    53
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -85,   -85,   130,    52,   -85,   -85,     3,   109,   -85,   -85,
     117,   -19,   -30,   -85,   -85,   -85,   -85,   107,   113,   -85,
     -85,   -64,   -85,   -85,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,   -34,    -8,   -84,   -85,   -85,   -85,   -85,   -85,   -85,
     -85,   -85,   -85,   -85,   -85,   -85,   -85
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     5,     6,     7,     8,    19,     9,    20,    10,    16,
      17,    25,    64,    65,    85,    11,    32,    33,   112,    87,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,    66,    49,    50,    51,    52,    53,    54,    55,    97,
      56,    57,    58,    59,    60,    61,    62
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      48,    18,    28,   125,    21,    98,    12,    67,    73,    74,
      72,    34,    99,    73,    74,    30,    35,    34,    22,    21,
      23,    83,    37,    38,    78,    79,    80,    31,    15,    78,
      82,    80,    24,    84,    73,    74,    75,    76,    68,    89,
      90,    91,    92,    31,    93,    94,    95,    96,    69,    81,
      78,    79,    80,   124,    41,    42,    73,    74,    43,    63,
     125,   125,    34,    44,   125,     1,     2,   100,     4,   128,
      86,   131,    78,    45,    46,    47,    15,    78,    41,    42,
     146,   147,    43,    39,   149,   126,    40,    44,   127,   129,
     111,   130,    26,   140,    27,   141,   142,    45,    46,    47,
      15,   132,   133,    30,   134,   143,     1,     2,     3,     4,
     124,   124,   135,   101,   124,   102,   103,   104,   105,     1,
       2,     3,     4,    41,    42,   136,   137,    43,    69,   106,
     138,   148,    44,   139,   107,    14,    36,   108,   109,   110,
      29,     0,    45,    46,    47,    15,   101,    71,   102,   103,
     104,   105,    70,     0,     0,     0,    41,    42,     0,     0,
      43,    69,     0,     0,     0,    44,     0,   107,     0,     0,
     108,   109,     0,     0,     0,    45,    46,    47,    15,    73,
      74,    75,    76,    73,    74,    75,    76,    77,    73,    74,
      75,    76,     0,    88,     0,    78,    79,    80,   144,    78,
      79,    80,     0,     0,    78,    79,    80,    73,    74,    75,
      76,    73,    74,    75,     0,     0,     0,   145,     0,     0,
       0,    13,     0,    78,    79,    80,     0,    78,    79,    80,
       1,     2,     3,     4
};

static const yytype_int16 yycheck[] =
{
      34,     9,    21,    87,    12,    19,     3,    37,    13,    14,
      44,    20,    26,    13,    14,    23,    25,    20,    24,    27,
      26,    55,    25,    31,    29,    30,    31,    24,    35,    29,
      49,    31,    22,    63,    13,    14,    15,    16,    20,    73,
      74,    75,    76,    40,    78,    79,    80,    81,    18,    22,
      29,    30,    31,    87,    13,    14,    13,    14,    17,    18,
     144,   145,    20,    22,   148,     9,    10,    86,    12,    99,
      21,   105,    29,    32,    33,    34,    35,    29,    13,    14,
     144,   145,    17,    23,   148,    23,    26,    22,    26,    22,
      87,    22,    24,   127,    26,   129,   130,    32,    33,    34,
      35,    24,    24,   111,    24,   139,     9,    10,    11,    12,
     144,   145,    24,     3,   148,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    24,    24,    17,    18,    19,
      24,     4,    22,    25,    24,     5,    27,    27,    28,    87,
      23,    -1,    32,    33,    34,    35,     3,    40,     5,     6,
       7,     8,    39,    -1,    -1,    -1,    13,    14,    -1,    -1,
      17,    18,    -1,    -1,    -1,    22,    -1,    24,    -1,    -1,
      27,    28,    -1,    -1,    -1,    32,    33,    34,    35,    13,
      14,    15,    16,    13,    14,    15,    16,    21,    13,    14,
      15,    16,    -1,    23,    -1,    29,    30,    31,    23,    29,
      30,    31,    -1,    -1,    29,    30,    31,    13,    14,    15,
      16,    13,    14,    15,    -1,    -1,    -1,    23,    -1,    -1,
      -1,     0,    -1,    29,    30,    31,    -1,    29,    30,    31,
       9,    10,    11,    12
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     9,    10,    11,    12,    39,    40,    41,    42,    44,
      46,    53,    44,     0,    40,    35,    47,    48,    70,    43,
      45,    70,    24,    26,    22,    49,    24,    26,    49,    48,
      70,    44,    54,    55,    20,    25,    45,    25,    70,    23,
      26,    13,    14,    17,    22,    32,    33,    34,    69,    70,
      71,    72,    73,    74,    75,    76,    78,    79,    80,    81,
      82,    83,    84,    18,    50,    51,    69,    50,    20,    18,
      56,    55,    69,    13,    14,    15,    16,    21,    29,    30,
      31,    22,    49,    69,    50,    52,    21,    57,    23,    69,
      69,    69,    69,    69,    69,    69,    69,    77,    19,    26,
      49,     3,     5,     6,     7,     8,    19,    24,    27,    28,
      41,    44,    56,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    71,    23,    26,    50,    22,
      22,    69,    24,    24,    24,    24,    24,    24,    24,    25,
      69,    69,    69,    69,    23,    23,    59,    59,     4,    59
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    38,    39,    39,    40,    40,    41,    41,    42,    43,
      43,    44,    44,    44,    45,    46,    47,    47,    48,    48,
      49,    49,    50,    50,    51,    52,    52,    52,    53,    54,
      54,    54,    55,    55,    56,    57,    57,    58,    58,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      60,    61,    62,    62,    63,    64,    65,    66,    66,    67,
      68,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    70,    71,    72,    72,    72,    73,    73,    74,    75,
      76,    76,    76,    77,    77,    77,    78,    79,    79,    80,
      81,    82,    83,    84
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     1,     1,     1,     1,     1,     4,     1,
       3,     1,     1,     1,     4,     3,     1,     3,     2,     4,
       0,     4,     1,     1,     3,     0,     1,     3,     6,     0,
       1,     3,     2,     5,     3,     0,     2,     1,     1,     2,
       1,     2,     1,     1,     1,     2,     2,     2,     2,     2,
       3,     1,     5,     7,     5,     1,     1,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     2,     3,     1,     1,     1,     1,     2,     4,
       1,     1,     1,     0,     1,     3,     3,     3,     3,     3,
       3,     3,     3,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* COMPUNITS: COMPUNITS COMPUNIT  */
#line 125 "parser.yy"
                              {
	(yyval.root) = (yyvsp[-1].root);
	(yyval.root)->list.emplace_back((yyvsp[0].compunit));
	root = (yyval.root);
}
#line 1289 "parser.cpp"
    break;

  case 3: /* COMPUNITS: COMPUNIT  */
#line 130 "parser.yy"
           {
	(yyval.root) = new AST::CompUnits(yyget_lineno());
	(yyval.root)->list.emplace_back((yyvsp[0].compunit));
	root = (yyval.root);
}
#line 1299 "parser.cpp"
    break;

  case 4: /* COMPUNIT: DECL  */
#line 137 "parser.yy"
                {
	(yyval.compunit) = static_cast<AST::CompUnit*>((yyvsp[0].decl));
}
#line 1307 "parser.cpp"
    break;

  case 5: /* COMPUNIT: FUNCDEF  */
#line 140 "parser.yy"
          {
	(yyval.compunit) = static_cast<AST::CompUnit*>((yyvsp[0].funcdef));
}
#line 1315 "parser.cpp"
    break;

  case 6: /* DECL: CONSTDECL  */
#line 145 "parser.yy"
                {
	(yyval.decl) = static_cast<AST::Decl*>((yyvsp[0].constdecl));
}
#line 1323 "parser.cpp"
    break;

  case 7: /* DECL: VARDECL  */
#line 148 "parser.yy"
          {
	(yyval.decl) = static_cast<AST::Decl*>((yyvsp[0].vardecl));
}
#line 1331 "parser.cpp"
    break;

  case 8: /* CONSTDECL: CONST BTYPE CONSTDEFS SEMICOLON  */
#line 153 "parser.yy"
                                           {
	(yyval.constdecl) = new AST::ConstDecl((yyvsp[-2].btype), (yyvsp[-1].constdefs), yyget_lineno());
}
#line 1339 "parser.cpp"
    break;

  case 9: /* CONSTDEFS: CONSTDEF  */
#line 158 "parser.yy"
                    {
	(yyval.constdefs) = new AST::ConstDefs(yyget_lineno());
	(yyval.constdefs)->list.emplace_back((yyvsp[0].constdef));
}
#line 1348 "parser.cpp"
    break;

  case 10: /* CONSTDEFS: CONSTDEFS COMMA CONSTDEF  */
#line 162 "parser.yy"
                           {
	(yyval.constdefs) = (yyvsp[-2].constdefs);
	(yyvsp[-2].constdefs)->list.emplace_back((yyvsp[0].constdef));
}
#line 1357 "parser.cpp"
    break;

  case 11: /* BTYPE: INT  */
#line 168 "parser.yy"
           {
	(yyval.btype) = AST::type::INT;
}
#line 1365 "parser.cpp"
    break;

  case 12: /* BTYPE: FLOAT  */
#line 171 "parser.yy"
        {
	(yyval.btype) = AST::type::FLOAT;
}
#line 1373 "parser.cpp"
    break;

  case 13: /* BTYPE: VOID  */
#line 174 "parser.yy"
       {
	(yyval.btype) = AST::type::VOID;
}
#line 1381 "parser.cpp"
    break;

  case 14: /* CONSTDEF: IDEXP ARR_INDEX EQ INITVAL  */
#line 179 "parser.yy"
                                     {
	(yyval.constdef) = new AST::ConstDef((yyvsp[-3].idexp), (yyvsp[-2].arr_index), (yyvsp[0].initval), yyget_lineno());
}
#line 1389 "parser.cpp"
    break;

  case 15: /* VARDECL: BTYPE VARDEFS SEMICOLON  */
#line 184 "parser.yy"
                                 {
	(yyval.vardecl) = new AST::VarDecl((yyvsp[-2].btype), (yyvsp[-1].vardefs), yyget_lineno());
}
#line 1397 "parser.cpp"
    break;

  case 16: /* VARDEFS: VARDEF  */
#line 189 "parser.yy"
                {
	(yyval.vardefs) = new AST::VarDefs(yyget_lineno());
	(yyval.vardefs)->list.emplace_back((yyvsp[0].vardef));
}
#line 1406 "parser.cpp"
    break;

  case 17: /* VARDEFS: VARDEFS COMMA VARDEF  */
#line 193 "parser.yy"
                       {
	(yyval.vardefs) = (yyvsp[-2].vardefs);
	(yyval.vardefs)->list.emplace_back((yyvsp[0].vardef));
}
#line 1415 "parser.cpp"
    break;

  case 18: /* VARDEF: IDEXP ARR_INDEX  */
#line 199 "parser.yy"
                        {
	(yyval.vardef) = new AST::VarDef((yyvsp[-1].idexp), (yyvsp[0].arr_index), nullptr, yyget_lineno());
}
#line 1423 "parser.cpp"
    break;

  case 19: /* VARDEF: IDEXP ARR_INDEX EQ INITVAL  */
#line 202 "parser.yy"
                             {
	(yyval.vardef) = new AST::VarDef((yyvsp[-3].idexp), (yyvsp[-2].arr_index), (yyvsp[0].initval), yyget_lineno());
}
#line 1431 "parser.cpp"
    break;

  case 20: /* ARR_INDEX: %empty  */
#line 207 "parser.yy"
            {
	(yyval.arr_index) = new AST::Arr_Index(yyget_lineno());
}
#line 1439 "parser.cpp"
    break;

  case 21: /* ARR_INDEX: ARR_INDEX LBRACKET EXP RBRACKET  */
#line 210 "parser.yy"
                                  {
	(yyval.arr_index) = (yyvsp[-3].arr_index);
	(yyval.arr_index)->list.emplace_back((yyvsp[-1].exp));
}
#line 1448 "parser.cpp"
    break;

  case 22: /* INITVAL: EXP  */
#line 216 "parser.yy"
             {
	(yyval.initval) = (yyvsp[0].exp);
}
#line 1456 "parser.cpp"
    break;

  case 23: /* INITVAL: ARR_INIT  */
#line 219 "parser.yy"
           {
	(yyval.initval) = (yyvsp[0].arr_init);
}
#line 1464 "parser.cpp"
    break;

  case 24: /* ARR_INIT: LBRACE INITVALS RBRACE  */
#line 224 "parser.yy"
                                 {
	(yyval.arr_init) = (yyvsp[-1].initvals);
}
#line 1472 "parser.cpp"
    break;

  case 25: /* INITVALS: %empty  */
#line 229 "parser.yy"
          {
	(yyval.initvals) = new AST::InitVals(yyget_lineno());
}
#line 1480 "parser.cpp"
    break;

  case 26: /* INITVALS: INITVAL  */
#line 232 "parser.yy"
          {
	(yyval.initvals) = new AST::InitVals((yyvsp[0].initval), yyget_lineno());
}
#line 1488 "parser.cpp"
    break;

  case 27: /* INITVALS: INITVALS COMMA INITVAL  */
#line 235 "parser.yy"
                         {
	(yyval.initvals) = (yyvsp[-2].initvals);
	(yyval.initvals)->list.emplace_back((yyvsp[0].initval));
}
#line 1497 "parser.cpp"
    break;

  case 28: /* FUNCDEF: BTYPE IDEXP LPAREN PARAMETERS RPAREN BLOCK  */
#line 241 "parser.yy"
                                                    {
	(yyval.funcdef) = new AST::FuncDef((yyvsp[-5].btype), (yyvsp[-4].idexp), (yyvsp[-2].parameters), (yyvsp[0].block), yyget_lineno());
}
#line 1505 "parser.cpp"
    break;

  case 29: /* PARAMETERS: %empty  */
#line 246 "parser.yy"
             {
	(yyval.parameters) = new AST::Parameters(yyget_lineno());
}
#line 1513 "parser.cpp"
    break;

  case 30: /* PARAMETERS: PARAMETER  */
#line 249 "parser.yy"
            {
	(yyval.parameters) = new AST::Parameters((yyvsp[0].parameter), yyget_lineno());
}
#line 1521 "parser.cpp"
    break;

  case 31: /* PARAMETERS: PARAMETERS COMMA PARAMETER  */
#line 252 "parser.yy"
                             {
	(yyval.parameters) = (yyvsp[-2].parameters);
	(yyval.parameters)->list.emplace_back((yyvsp[0].parameter));
}
#line 1530 "parser.cpp"
    break;

  case 32: /* PARAMETER: BTYPE IDEXP  */
#line 258 "parser.yy"
                       {
	(yyval.parameter) = new AST::Parameter((yyvsp[-1].btype), (yyvsp[0].idexp), new AST::Arr_Index(yyget_lineno()), yyget_lineno());
}
#line 1538 "parser.cpp"
    break;

  case 33: /* PARAMETER: BTYPE IDEXP LBRACKET RBRACKET ARR_INDEX  */
#line 261 "parser.yy"
                                          {
	auto x = new AST::Arr_Index(yyget_lineno());
	x->list.emplace_back(new AST::IntNum(1, yyget_lineno()));
	for (const auto &y:(yyvsp[0].arr_index)->list) {
		x->list.emplace_back(y);
	}
	(yyval.parameter) = new AST::Parameter((yyvsp[-4].btype), (yyvsp[-3].idexp), x, yyget_lineno());
}
#line 1551 "parser.cpp"
    break;

  case 34: /* BLOCK: LBRACE BLOCKITEMS RBRACE  */
#line 271 "parser.yy"
                                {
	(yyval.block) = new AST::Block((yyvsp[-1].blockitems), yyget_lineno());
}
#line 1559 "parser.cpp"
    break;

  case 35: /* BLOCKITEMS: %empty  */
#line 276 "parser.yy"
             {
	(yyval.blockitems) = new AST::BlockItems(yyget_lineno());
}
#line 1567 "parser.cpp"
    break;

  case 36: /* BLOCKITEMS: BLOCKITEMS BLOCKITEM  */
#line 278 "parser.yy"
                         {
	(yyval.blockitems) = (yyvsp[-1].blockitems);
	(yyval.blockitems)->list.emplace_back((yyvsp[0].blockitem));
}
#line 1576 "parser.cpp"
    break;

  case 37: /* BLOCKITEM: DECL  */
#line 284 "parser.yy"
                {
	(yyval.blockitem) = (yyvsp[0].decl);
}
#line 1584 "parser.cpp"
    break;

  case 38: /* BLOCKITEM: STMT  */
#line 286 "parser.yy"
         {
	(yyval.blockitem) = (yyvsp[0].stmt);
}
#line 1592 "parser.cpp"
    break;

  case 39: /* STMT: ASSIGNSTMT SEMICOLON  */
#line 291 "parser.yy"
                           {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1600 "parser.cpp"
    break;

  case 40: /* STMT: SEMICOLON  */
#line 294 "parser.yy"
            {
	(yyval.stmt) = new AST::ExpStmt(new AST::IntNum(0, yyget_lineno()), yyget_lineno());
}
#line 1608 "parser.cpp"
    break;

  case 41: /* STMT: EXPSTMT SEMICOLON  */
#line 297 "parser.yy"
                    {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1616 "parser.cpp"
    break;

  case 42: /* STMT: BLOCK  */
#line 300 "parser.yy"
        {
	(yyval.stmt) = (yyvsp[0].block);
}
#line 1624 "parser.cpp"
    break;

  case 43: /* STMT: IFSTMT  */
#line 303 "parser.yy"
         {
	(yyval.stmt) = (yyvsp[0].stmt);
}
#line 1632 "parser.cpp"
    break;

  case 44: /* STMT: WHILESTMT  */
#line 306 "parser.yy"
            {
	(yyval.stmt) = (yyvsp[0].stmt);
}
#line 1640 "parser.cpp"
    break;

  case 45: /* STMT: BREAKSTMT SEMICOLON  */
#line 309 "parser.yy"
                      {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1648 "parser.cpp"
    break;

  case 46: /* STMT: CONTINUESTMT SEMICOLON  */
#line 312 "parser.yy"
                         {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1656 "parser.cpp"
    break;

  case 47: /* STMT: RETURNSTMT SEMICOLON  */
#line 315 "parser.yy"
                       {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1664 "parser.cpp"
    break;

  case 48: /* STMT: STARTTIMESTMT SEMICOLON  */
#line 318 "parser.yy"
                          {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1672 "parser.cpp"
    break;

  case 49: /* STMT: STOPTIMESTMT SEMICOLON  */
#line 321 "parser.yy"
                         {
	(yyval.stmt) = (yyvsp[-1].stmt);
}
#line 1680 "parser.cpp"
    break;

  case 50: /* ASSIGNSTMT: LVAL EQ EXP  */
#line 326 "parser.yy"
                        {
	(yyval.stmt) = new AST::AssignStmt((yyvsp[-2].lval), (yyvsp[0].exp), yyget_lineno());
}
#line 1688 "parser.cpp"
    break;

  case 51: /* EXPSTMT: EXP  */
#line 331 "parser.yy"
             {
	(yyval.stmt) = new AST::ExpStmt((yyvsp[0].exp), yyget_lineno());
}
#line 1696 "parser.cpp"
    break;

  case 52: /* IFSTMT: IF LPAREN EXP RPAREN STMT  */
#line 336 "parser.yy"
                                              {
	(yyval.stmt) = new AST::IfStmt((yyvsp[-2].exp), (yyvsp[0].stmt), nullptr, yyget_lineno());
}
#line 1704 "parser.cpp"
    break;

  case 53: /* IFSTMT: IF LPAREN EXP RPAREN STMT ELSE STMT  */
#line 339 "parser.yy"
                                      {
	(yyval.stmt) = new AST::IfStmt((yyvsp[-4].exp), (yyvsp[-2].stmt), (yyvsp[0].stmt), yyget_lineno());
}
#line 1712 "parser.cpp"
    break;

  case 54: /* WHILESTMT: WHILE LPAREN EXP RPAREN STMT  */
#line 344 "parser.yy"
                                        {
	(yyval.stmt) = new AST::WhileStmt((yyvsp[-2].exp), (yyvsp[0].stmt), yyget_lineno());
}
#line 1720 "parser.cpp"
    break;

  case 55: /* BREAKSTMT: BREAK  */
#line 349 "parser.yy"
                 {
	(yyval.stmt) = new AST::BreakStmt(yyget_lineno());
}
#line 1728 "parser.cpp"
    break;

  case 56: /* CONTINUESTMT: CONTINUE  */
#line 354 "parser.yy"
                       {
	(yyval.stmt) = new AST::ContinueStmt(yyget_lineno());
}
#line 1736 "parser.cpp"
    break;

  case 57: /* RETURNSTMT: RETURN  */
#line 359 "parser.yy"
                   {
	(yyval.stmt) = new AST::ReturnStmt(nullptr, yyget_lineno());
}
#line 1744 "parser.cpp"
    break;

  case 58: /* RETURNSTMT: RETURN EXP  */
#line 362 "parser.yy"
             {
	(yyval.stmt) = new AST::ReturnStmt((yyvsp[0].exp), yyget_lineno());
}
#line 1752 "parser.cpp"
    break;

  case 59: /* STARTTIMESTMT: STARTTIME  */
#line 367 "parser.yy"
                         {
	(yyval.stmt) = new AST::StarttimeStmt(yyget_lineno());
}
#line 1760 "parser.cpp"
    break;

  case 60: /* STOPTIMESTMT: STOPTIME  */
#line 372 "parser.yy"
                       {
	(yyval.stmt) = new AST::StoptimeStmt(yyget_lineno());
}
#line 1768 "parser.cpp"
    break;

  case 61: /* EXP: PRIMARYEXP  */
#line 377 "parser.yy"
                {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1776 "parser.cpp"
    break;

  case 62: /* EXP: UNARYEXP  */
#line 380 "parser.yy"
           {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1784 "parser.cpp"
    break;

  case 63: /* EXP: MULEXP  */
#line 383 "parser.yy"
         {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1792 "parser.cpp"
    break;

  case 64: /* EXP: ADDEXP  */
#line 386 "parser.yy"
         {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1800 "parser.cpp"
    break;

  case 65: /* EXP: RELEXP  */
#line 389 "parser.yy"
         {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1808 "parser.cpp"
    break;

  case 66: /* EXP: EQEXP  */
#line 392 "parser.yy"
        {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1816 "parser.cpp"
    break;

  case 67: /* EXP: LANDEXP  */
#line 395 "parser.yy"
          {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1824 "parser.cpp"
    break;

  case 68: /* EXP: LOREXP  */
#line 398 "parser.yy"
         {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1832 "parser.cpp"
    break;

  case 69: /* EXP: CALLEXP  */
#line 401 "parser.yy"
          {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1840 "parser.cpp"
    break;

  case 70: /* EXP: FORMATEXP  */
#line 404 "parser.yy"
            {
	(yyval.exp) = (yyvsp[0].exp);
}
#line 1848 "parser.cpp"
    break;

  case 71: /* IDEXP: ID  */
#line 409 "parser.yy"
          {
	(yyval.idexp) = new AST::IdExp((yyvsp[0].id), yyget_lineno());
}
#line 1856 "parser.cpp"
    break;

  case 72: /* LVAL: IDEXP ARR_INDEX  */
#line 414 "parser.yy"
                      {
	(yyval.lval) = new AST::Lval((yyvsp[-1].idexp), (yyvsp[0].arr_index), yyget_lineno());
}
#line 1864 "parser.cpp"
    break;

  case 73: /* PRIMARYEXP: LPAREN EXP RPAREN  */
#line 419 "parser.yy"
                              {
	(yyval.exp) = (yyvsp[-1].exp);
}
#line 1872 "parser.cpp"
    break;

  case 74: /* PRIMARYEXP: LVAL  */
#line 422 "parser.yy"
       {
	(yyval.exp) = (yyvsp[0].lval);
}
#line 1880 "parser.cpp"
    break;

  case 75: /* PRIMARYEXP: NUMBER  */
#line 425 "parser.yy"
         {
	(yyval.exp) = (yyvsp[0].number);
}
#line 1888 "parser.cpp"
    break;

  case 76: /* NUMBER: INTNUM  */
#line 430 "parser.yy"
               {
	(yyval.number) = new AST::IntNum((yyvsp[0].intnum), yyget_lineno());
}
#line 1896 "parser.cpp"
    break;

  case 77: /* NUMBER: FLOATNUM  */
#line 433 "parser.yy"
           {
	(yyval.number) = new AST::FloatNum((yyvsp[0].floatnum), yyget_lineno());
}
#line 1904 "parser.cpp"
    break;

  case 78: /* UNARYEXP: UNARYOP EXP  */
#line 438 "parser.yy"
                                  {
	(yyval.exp) = new AST::UnaryExp((yyvsp[-1].unaryop), (yyvsp[0].exp), yyget_lineno());
}
#line 1912 "parser.cpp"
    break;

  case 79: /* CALLEXP: IDEXP LPAREN EXPS RPAREN  */
#line 443 "parser.yy"
                                  {
	(yyval.exp) = new AST::CallExp((yyvsp[-3].idexp), (yyvsp[-1].exps), yyget_lineno());
}
#line 1920 "parser.cpp"
    break;

  case 80: /* UNARYOP: ADD  */
#line 448 "parser.yy"
             {
	(yyval.unaryop) = AST::unary_op::ADD;
}
#line 1928 "parser.cpp"
    break;

  case 81: /* UNARYOP: SUB  */
#line 451 "parser.yy"
      {
	(yyval.unaryop) = AST::unary_op::SUB;
}
#line 1936 "parser.cpp"
    break;

  case 82: /* UNARYOP: NOT  */
#line 454 "parser.yy"
      {
	(yyval.unaryop) = AST::unary_op::NOT;
}
#line 1944 "parser.cpp"
    break;

  case 83: /* EXPS: %empty  */
#line 459 "parser.yy"
       {
	(yyval.exps) = new AST::Exps(yyget_lineno());
}
#line 1952 "parser.cpp"
    break;

  case 84: /* EXPS: EXP  */
#line 462 "parser.yy"
      {
	(yyval.exps) = new AST::Exps((yyvsp[0].exp), yyget_lineno());
}
#line 1960 "parser.cpp"
    break;

  case 85: /* EXPS: EXPS COMMA EXP  */
#line 465 "parser.yy"
                 {
	(yyval.exps) = (yyvsp[-2].exps);
	(yyval.exps)->list.emplace_back((yyvsp[0].exp));
}
#line 1969 "parser.cpp"
    break;

  case 86: /* MULEXP: EXP MUL_OP EXP  */
#line 471 "parser.yy"
                       {
	(yyval.exp) = new AST::MulExp((yyvsp[-2].exp), (yyvsp[-1].mul_op), (yyvsp[0].exp), yyget_lineno());
}
#line 1977 "parser.cpp"
    break;

  case 87: /* ADDEXP: EXP ADD EXP  */
#line 476 "parser.yy"
                    {
	(yyval.exp) = new AST::AddExp((yyvsp[-2].exp), AST::add_op::ADD, (yyvsp[0].exp), yyget_lineno());
}
#line 1985 "parser.cpp"
    break;

  case 88: /* ADDEXP: EXP SUB EXP  */
#line 479 "parser.yy"
              {
	(yyval.exp) = new AST::AddExp((yyvsp[-2].exp), AST::add_op::SUB, (yyvsp[0].exp), yyget_lineno());
}
#line 1993 "parser.cpp"
    break;

  case 89: /* RELEXP: EXP REL_OP EXP  */
#line 484 "parser.yy"
                       {
	(yyval.exp) = new AST::RelExp((yyvsp[-2].exp), (yyvsp[-1].rel_op), (yyvsp[0].exp), yyget_lineno());
}
#line 2001 "parser.cpp"
    break;

  case 90: /* EQEXP: EXP EQUAL_OP EXP  */
#line 489 "parser.yy"
                        {
	(yyval.exp) = new AST::EqExp((yyvsp[-2].exp), (yyvsp[-1].equal_op), (yyvsp[0].exp), yyget_lineno());
}
#line 2009 "parser.cpp"
    break;

  case 91: /* LANDEXP: EXP LAND EXP  */
#line 494 "parser.yy"
                      {
	(yyval.exp) = new AST::LAndExp((yyvsp[-2].exp), (yyvsp[0].exp), yyget_lineno());
}
#line 2017 "parser.cpp"
    break;

  case 92: /* LOREXP: EXP LOR EXP  */
#line 499 "parser.yy"
                    {
	(yyval.exp) = new AST::LOrExp((yyvsp[-2].exp), (yyvsp[0].exp), yyget_lineno());
}
#line 2025 "parser.cpp"
    break;

  case 93: /* FORMATEXP: FORMAT  */
#line 504 "parser.yy"
                  {
	(yyval.exp) = new AST::FormatExp((yyvsp[0].format), yyget_lineno());
}
#line 2033 "parser.cpp"
    break;


#line 2037 "parser.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 508 "parser.yy"

