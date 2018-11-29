#!/usr/bin/python3

import argparse
import random


def gen_stupid(n, m):
    print("p td {} {}".format(n, m))
    ex = set()
    for i in range(m):
        u = random.randint(1, n)
        v = random.randint(1, n)
        while u == v or (u, v) in ex:
            u = random.randint(1, n)
            v = random.randint(1, n)
        print("{} {}".format(u, v))


def gen_of_sample(n, m):
    print("p td {} {}".format(n, m))
    possible = []
    for i in range(n):
        for j in range(i + 1, n):
            possible.append((i + 1, j + 1))
    for u, v in random.sample(possible, m):
        print("{} {}".format(u, v))


def gen(n, m, seed):
    print("c Random graph ({} vertices, {} edges, seed={})".format(n, m, seed))
    if 2 * m >= n * (n - 1) // 2:
        gen_of_sample(n, m)
    else:
        gen_stupid(n, m)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("vertices",
                        metavar="VERTICES",
                        type=int,
                        help="Number of vertices in the graph")
    parser.add_argument("edges",
                        metavar="EDGES",
                        type=int,
                        help="Number of edges in the graph")
    parser.add_argument("--seed",
                        metavar="SEED",
                        type=int,
                        default=0,
                        help="Random seed")

    args = parser.parse_args()

    random.seed(args.seed)

    gen(args.vertices, args.edges, args.seed)


if __name__ == "__main__":
    main()
