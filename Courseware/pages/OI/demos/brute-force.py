def P(n):
    D = [i for i in range(2, n) if n % i == 0]
    for s in (f'{i:0{n}b}' for i in range(2 ** n)):
        if s in (s[0:d] * (n // d) for d in D):
            yield s

def S(n):
    return sum(1 for _ in P(n))
