# Architecture

## Disassembler

The disassembler loads in a Java/Kotlin binary file and goes through the different JVM opcodes to create basic blocks (BB), and a control flow graph (CFG). 

## Decompiler

The decompiler takes the disassembled data and turns it into an AST that resembles Java code.