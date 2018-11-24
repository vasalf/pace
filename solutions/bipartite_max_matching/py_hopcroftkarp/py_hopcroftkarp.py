#!/usr/bin/python3

from hopcroftkarp import HopcroftKarp
import sys


def read_line():
    s = sys.stdin.readline()
    while s[0] == 'c':
        s = sys.stdin.readline()
    return s


def main():
    s = read_line().split()
    n, k, m = map(int, s[2:])
    graph = dict()
    for i in range(n + k):
        graph[i] = set()
    for i in range(m):
        u, v = map(int, read_line().split())
        u -= 1
        v -= 1
        v += n
        graph[u].add(v)
        graph[v].add(u)
    print(len(HopcroftKarp(graph).maximum_matching()) // 2)


if __name__ == "__main__":
    main()
