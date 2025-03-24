## Formatting / Code Style Guide

### File Structure and Access
* Place constants in the defs.h file to keep them organized
* Each class must have a corresponding `.h` and `.cc` file
* Try to keep library and file include statements in the appropriate `.h` file
* Keep member variables private, member functions may have any access specifier

### Documentation
* Place a comment at the top of each file with a brief explanation of it's purpose
* Place a comment surrounded by `/*** comment ***/` above each function definition with an explanation of what it does
* Space function definitions out by two newlines
* Functions with one line definitions may be defined in the header file with the `inline` keyword
* Function prototypes should have their parameters named appropriately in the `.h` file
* The order in which functions are declared in the `.h` file should be the same as the order they are defined in the `.cc` file

### Styling
* Spacing should be 4 spaces in all files
* Space out big blocks of code with newlines (e.g. if-statements, loops)
* Keep functions definitions two newlines apart from each other
* Keep spaces between operators, unless its a very small equation (e.g. `++i`, `i+1`)
* Opening curly brackets must be kept on the same line with one space, do not use a newline
* When passing by reference or by pointer, place the symbol after the data type (e.g. `obj& foo`, `obj* bar`)
* Define static member variables in their `.cc` file
* Define default member variable values in the constructor

### Naming Conventions
* Functions must use Pascal Case (e.g. `MyFunction()`)
* Constants use Snake Case in caps (e.g. `CONSTANT_VAR`)
* Standard variables must use Snake Case, may have a underscore at the end (e.g. `temp_var`, `member_var_`)
* File names must use Snake Case (e.g. `game_object.cc`)

