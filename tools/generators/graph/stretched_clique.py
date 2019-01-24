#!/usr/bin/python3

import argparse


def gen(n):
    print("c Clique of {} vertices with a leaf added to each vertice".format(n))

    print("p td", 2 * n, n + n * (n - 1) // 2)
    for i in range(n):
        print(i + 1, i + 1 + n)
        for j in range(i):
            print(i + 1, j + 1)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("vertices",
                        metavar="VERTICES",
                        type=int,
                        help="Number of vertices in the clique")
    args = parser.parse_args()

    gen(args.vertices)


if __name__ == "__main__":
    main()
