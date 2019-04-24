#!/usr/bin/python3

import argparse
import random

def gen(n, d, seed):
    print("c Regular graph ({} vertices, {} degree each, seed={})".format(n, d, seed))
    print("p td {} {}".format(n, n * d // 2))
    not_full = set(range(n))
    deg = [0 for i in range(n)]
    for i in range(n * d // 2):
        a = random.choice(list(not_full))
        b = random.choice(list(not_full))
        print(a + 1, b + 1)
        deg[a] += 1
        deg[b] += 1
        if deg[a] == d:
            not_full.discard(a)
        if deg[b] == d:
            not_full.discard(b)


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
