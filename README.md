# SysY2022-ARMv7-Compiler
An optimizing compiler for the SysY2022 language that targets 32-bit ARMv7 assembly. It features a full front end (lexer/parser, AST, semantic analysis), an MIR+SSA-based middle end with classic optimizations, and a practical ARM backend with register allocation, producing linkable .s output for the SysY runtime.
