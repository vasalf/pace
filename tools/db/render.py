#!/usr/bin/python3
# *-* coding: utf-8 *-*


import argparse
import config
import dbtools
import rendertools


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config',
                        metavar="CONFIG",
                        type=str,
                        default="conf/db_default.json",
                        help='Path to configuration file')
    args = parser.parse_args()

    conf = config.Config(args.config)
    db = dbtools.Database(conf)
    rendertools.render(db)


if __name__ == "__main__":
    main()
