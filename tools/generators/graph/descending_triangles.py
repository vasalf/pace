#!/usr/bin/python3

import argparse


def gen(layers):
    print("c {} layers of descending triangles".format(layers))

    edges = []
    prev = 0
    lprev = 0
    for i in range(layers, 0, -1):
        for k in range(i):
            for j in range(lprev, prev):
                edges.append((prev + k + 1, j + 1))
        prev += i
        if i + 1 <= layers:
            lprev += i + 1

    print("p td", prev, len(edges))
    for u, v in edges:
        print(u, v)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("layers",
                        metavar="LAYERS",
                        type=int,
                        help="Number of layers in the graph")
    args = parser.parse_args()

    gen(args.layers)


if __name__ == "__main__":
    main()
