package main

import (
    "fmt"
    "time"
)

func main() {
	// As if we have created a thread running spinner.
    go spinner(100 * time.Millisecond)

	// Slow sequential computation
	const n = 45
    fmt.Printf("\rFibonacci(%d) = %d\n", n, fib(n))
}

func spinner(delay time.Duration) {
    for {
        for _, r := range `-\|/` {
            fmt.Printf("\r%c", r)
            time.Sleep(delay)
        }
    }
}

func fib(x int) int {
	// This is an exponential-time implementation.

    if x < 2 {
		return x
	}

    return fib(x - 1) + fib(x - 2)
}
