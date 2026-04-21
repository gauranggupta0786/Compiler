# Compiler Project (Group 22)

A modular implementation of a **compiler front-end** for a custom programming language.  
This project includes components for **lexical analysis, parsing (LL(1)), grammar processing, and utility structures** such as tries and string handling.

---

##  Team Members

| Name            | ID              |
|-----------------|-----------------|
| Archit Garg     | 2022B2A71602PP  |
| Aadit Shah      | 2022B3A70612P   |
| Rishant Sharma  | 2022B5A70814P   |
| Rishab Chetal   | 2023A7PS0015P   |
| Gaurang Gupta   | 2023A7PS0648P   |
| Ansh Bansal     | 2023A7PS0645P   |

---

##  Features

-  **Lexical Analyzer**
  - Tokenizes input source code
  - Handles identifiers, keywords, numbers, operators, etc.
  - Uses **Trie-based keyword lookup** for efficiency

-  **Parser (LL(1))**
  - Implements grammar using recursive rules
  - Builds parsing structure based on FIRST and FOLLOW sets
  - Detects syntax errors

-  **Grammar Handling**
  - Predefined grammar using structured rules
  - FIRST and FOLLOW set computation
  - Handles epsilon productions and dependencies

-  **Utilities**
  - Custom string comparison functions
  - Trie implementation for keyword recognition
  - Helper functions for grammar processing

---
##  Project Structure

1. **String Utilities**
   - (a) `string.c` – Implementation of string comparison logic  
   - (b) `string.h` – Header file with function declarations  

2. **Trie (Keyword Recognition)**
   - (a) `trie.c` – Trie implementation for keyword insertion and search  
   - (b) `trie.h` – Trie structure definitions and function prototypes  

3. **Grammar & Utility Functions**
   - (a) `utils.c` – FIRST/FOLLOW computation and grammar processing  
   - (b) `utils.h` – Function declarations for grammar utilities  

4. **Core Definitions**
   - (a) `lexerDef.h` – Token definitions (used by lexer and parser)  
   - (b) `parserDef.h` – Grammar structures and parser-related definitions  

5. **Test Cases**
   - (a) `t1.txt` – Lexer testing (invalid syntax)  
   - (b) `t2.txt` – Sum of N numbers  
   - (c) `t3.txt` – Record handling and averaging  
   - (d) `t4.txt` – Arithmetic expressions and conditionals  
   - (e) `t5.txt` – Complex types (union, record, typedef)  
   - (f) `t6.txt` – Syntax error test case  
   - (g) `testcase.txt` – If-else (maximum of two numbers)  
   - (h) `testcase7.txt` – Average of two numbers  

6. Executable / Driver 
   - (a) `driver.c` – Entry point integrating lexer and parser   

   
---

##  Core Components

### 1. Trie (Keyword Recognition)
- Efficient keyword lookup using character-based tree
- Supports:
  - `insertKeyword()`
  - `searchKeyword()`

### 2. FIRST & FOLLOW Computation
- Recursive computation of FIRST sets
- Dependency-based resolution for FOLLOW sets
- Handles:
  - Epsilon productions
  - Multi-rule grammars

### 3. Grammar Specification
- Grammar is hardcoded using structures
- Each non-terminal maps to production rules
- Supports:
  - Terminals and Non-terminals
  - Multiple productions
  - Epsilon handling

### 4. String Utilities
- Custom implementation of string comparison
- Avoids use of standard library functions

---

##  Test Cases

| File           | Description |
|----------------|------------|
| `t1.txt`       | Lexer stress test (invalid syntax) |
| `t2.txt`       | Sum of N numbers |
| `t3.txt`       | Record handling & averaging |
| `t4.txt`       | Arithmetic + conditionals |
| `t5.txt`       | Complex types (union, record, typedef) |
| `t6.txt`       | Syntax error test |
| `testcase.txt` | If-else maximum program |
| `testcase7.txt`| Average of two numbers |

---

##  How to Run

### 1. Compile
gcc *.c -o compiler

### 2.Run
./compiler <input_file>

 Example:
 ./compiler t2.txt
