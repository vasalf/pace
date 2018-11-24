#!/usr/bin/python3

import argparse
import random

def gen_stupid(n, k, m):
    print("p bi {} {} {}".format(n, k, m))
    ex = set()
    for i in range(m):
        u = random.randint(1, n)
        v = random.randint(1, k)
        while (u, v) in ex:
            u = random.randint(1, n)
            v = random.randint(1, k)
        print("{} {}".format(u, v))


def gen_of_sample(n, k, m):
    print("p bi {} {} {}".format(n, k, m))
    possible = []
    for i in range(n):
        for j in range(k):
            possible.append((i + 1, j + 1))
    for u, v in random.sample(possible, m):
        print("{} {}".format(u, v))


def gen(n, k, m, seed):
    print("c Random bipartite graph ({} + {} vertices, {} edges, seed={})".format(n, k, m, seed))
    if 2 * m >= n * k:
        gen_of_sample(n, k, m)
    else:
        gen_stupid(n, k, m)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("left",
                        metavar="LEFT",
                        type=int,
                        help="Number of vertices in the left part")
    parser.add_argument("right",
                        metavar="RIGHT",
                        type=int,
                        help="Number of vertices in the right part")
    parser.add_argument("edges",
                        metavar="EDGES",
                        type=int,
                        help="Number of edges")
    parser.add_argument("--seed",
                        metavar="SEED",
                        type=int,
                        default=0,
                        help="Random seed")
    args = parser.parse_args()

    random.seed(args.seed)

    gen(args.left, args.right, args.edges, args.seed)


if __name__ == "__main__":
    main()
