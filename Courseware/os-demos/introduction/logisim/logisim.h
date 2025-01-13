#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

// Wires
// A wire in digital circuits is used to connect different components
// together, allowing the transmission of electrical signals between them.
// A wire is represented as a boolean value (true or false), which
// corresponds to high (1) and low (0) voltage levels in a real circuit.
typedef bool wire;

// Flip-flops
// A basic memory element in digital circuits, capable of storing one bit
// of data. It has an input and an output (wires), and it maintains its
// output value until the input changes and a clock signal is received.
typedef struct {
    bool value;  // The current value stored in the flip-flop
    wire *in;    // Pointer to the input wire
    wire *out;   // Pointer to the output wire
} reg;

// Logical gates from NAND
// NAND gate is a fundamental building block in digital electronics. Using
// NAND gates, one can construct any other logical operation.

// NAND gate: Returns true unless both inputs are true.
#define NAND(X, Y)  (!((X) && (Y)))

// NOT gate: Inverts the input.
#define NOT(X)      (NAND(X, 1))

// AND gate: Returns true only if both inputs are true.
#define AND(X, Y)   (NOT(NAND(X, Y)))

// OR gate: Returns true if at least one input is true.
#define OR(X, Y)    (NAND(NOT(X), NOT(Y)))

// Clock cycles
// In digital circuits, a clock signal is used to synchronize operations.
// This infinite loop mimicks the continuous nature of a clock signal in
// a real circuit. We assume that all flip-flops are updated simultaneously
// on the end of a cycle.
#define CLOCK_CYCLE while (1)
