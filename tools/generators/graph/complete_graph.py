#!/usr/bin/python3

import argparse


def gen(vertices):
    print("c Complete graph on {} vertices".format(vertices))
    print("p td", vertices, vertices * (vertices - 1) // 2)
    for u in range(vertices):
        for v in range(u):
            print(u + 1, v + 1)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("vertices",
                        metavar="VERTICES",
                        type=int,
                        help="Number of vertices in the graph")
    args = parser.parse_args()

    gen(args.vertices)


if __name__ == "__main__":
    main()
