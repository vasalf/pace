#!/usr/bin/python3
# *-* coding: utf-8 *-*

import argparse
import json
import os


def generate(config):
    for test in config["tests"]:
        os.system(test["command"] + " >" + test["output_file"])


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("config",
                        metavar="CONFIG",
                        type=str,
                        help="Path to configuration file")
    args = parser.parse_args()

    with open(args.config, 'r') as conf:
        generate(json.load(conf))


if __name__ == "__main__":
    main()
