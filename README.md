# Interpreter
An interpreter for a simple (unofficial) language written in C++

***

These are the grammar rules of the language:

![image](rules.JPG)

***

The lexical analyzer converts a sequences of characters into a sequence of tokens. The resulting tokens then get passed into the parser (a recursive descent, top-down parsing implementation) which builds the parse tree from the read tokens. Finally, a series of static and dynamic (runtime) checks are performed on the nodes of the tree.
