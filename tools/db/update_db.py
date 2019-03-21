#!/usr/bin/python3
# *-* coding: utf-8 *-*


import argparse
import config
import dbtools
import instance


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config',
                        metavar="CONFIG",
                        type=str,
                        default="conf/db_default.json",
                        help='Path to configuration file')
    parser.add_argument('--threads',
                        metavar="THREADS",
                        type=int,
                        default=1,
                        help='Number of threads to be launched')
    args = parser.parse_args()

    conf = config.Config(args.config)
    db = dbtools.Database(conf)
    db.update_all(args.threads)
    db.write()


if __name__ == "__main__":
    main()
