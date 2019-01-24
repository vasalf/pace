#!/usr/bin/python3

import argparse


def gen(n, m, k):
    print("c Independent set of size {} fully connected to {} vertices of a clique of size {}".format(n, k, m))
    print("p td {} {}".format(n + m, n * k + m * (m - 1) // 2))
    for u in range(n):
        for v in range(k):
            print(u + 1, n + v + 1)
    for u in range(m):
        for v in range(u + 1, m):
            print(n + u + 1, n + v + 1)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("independent",
                        metavar="INDEPENDENT",
                        type=int,
                        help="Number of vertices in the independent set")
    parser.add_argument("intermediate",
                        metavar="INTERMEDIATE",
                        type=int,
                        help="Number of clique vertices connected to the independent set")
    parser.add_argument("clique",
                        metavar="CLIQUE",
                        type=int,
                        help="Number of vertices in the clique")

    args = parser.parse_args()
    gen(args.independent, args.clique, args.intermediate)


if __name__ == "__main__":
    main()
