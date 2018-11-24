#!/usr/bin/python3
# *-* coding: utf-8 *-*

import argparse
import abc
import datetime
import json
import subprocess
import tabulate


class Test:
    def __init__(self, config):
        self.filename = config["filename"]
        self.name = config.get("name", self.filename)


class Solution:
    def __init__(self, config):
        self.executable = config["executable"]
        self.name = config.get("name", self.executable)


class Config:
    def __init__(self, data):
        self.timeout = data.get("timeout", -1)
        self.tests = list(map(Test, data["tests"]))
        self.solutions = list(map(Solution, data["solutions"]))


class TestResult(abc.ABC):
    @abc.abstractmethod
    def __str__(self):
        pass


class TestSuccess(TestResult):
    def __init__(self, time_ms):
        self.time_ms = time_ms

    def __str__(self):
        seconds = self.time_ms // 1000
        msecs = self.time_ms % 1000
        return "{}.{:>03d} s".format(seconds, msecs)


class TestTimeout(TestResult):
    def __str__(self):
        return "TO"


def run(config, solution, test):
    with open(test.filename, "r") as test_in:
        tstart = datetime.datetime.now()
        proc = subprocess.Popen([solution.executable],
                                stdin=test_in,
                                stdout=subprocess.DEVNULL)
        if config.timeout > 0:
            try:
                proc.wait(timeout=config.timeout)
            except subprocess.TimeoutExpired:
                proc.kill()
                return TestTimeout()
        else:
            proc.wait()
        tend = datetime.datetime.now()
        return TestSuccess((tend - tstart) // datetime.timedelta(microseconds=1000))


def run_all(config):
    data = []
    headers = ["test"] + list(map(lambda x: x.name, config.solutions))
    for test in config.tests:
        row = [test.name]
        for solution in config.solutions:
            row.append(run(config, solution, test))
        data.append(row)
    return tabulate.tabulate(data, headers=headers)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('config',
                        metavar="CONFIG",
                        type=str,
                        help='Path to configuration file')
    args = parser.parse_args()

    with open(args.config, 'r') as conf:
        print(run_all(Config(json.load(conf))))


if __name__ == "__main__":
    main()
