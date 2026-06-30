from __future__ import annotations

from collections import Counter
from math import gcd
from random import randrange


def is_prime(n: int) -> bool:
    if n < 2:
        return False
    small = (2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37)
    for p in small:
        if n % p == 0:
            return n == p
    d = n - 1
    s = 0
    while d % 2 == 0:
        s += 1
        d //= 2
    for a in (2, 325, 9375, 28178, 450775, 9780504, 1795265022):
        if a % n == 0:
            continue
        x = pow(a, d, n)
        if x in (1, n - 1):
            continue
        for _ in range(s - 1):
            x = x * x % n
            if x == n - 1:
                break
        else:
            return False
    return True


def _rho(n: int) -> int:
    if n % 2 == 0:
        return 2
    while True:
        c = randrange(1, n)
        x = randrange(0, n)
        y = x
        d = 1
        while d == 1:
            x = (x * x + c) % n
            y = (y * y + c) % n
            y = (y * y + c) % n
            d = gcd(abs(x - y), n)
        if d != n:
            return d


def prime_factors(n: int) -> list[int]:
    if n <= 1:
        return []
    if is_prime(n):
        return [n]
    d = _rho(n)
    return sorted(prime_factors(d) + prime_factors(n // d))


def factorize(n: int) -> list[tuple[int, int]]:
    return sorted(Counter(prime_factors(n)).items())


def divisors(n: int) -> list[int]:
    result = [1]
    for p, e in factorize(n):
        current = result[:]
        q = 1
        for _ in range(e):
            q *= p
            result += [x * q for x in current]
    return sorted(result)
