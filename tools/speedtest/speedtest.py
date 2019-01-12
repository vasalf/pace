#!/usr/bin/python3
# *-* coding: utf-8 *-*

import argparse
import abc
import datetime
import json
import os
import os.path
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

        if "tests" in data:
            self.tests = list(map(Test, data["tests"]))
        else:
            testdir = data["testdir"]
            tests = [f for f in os.listdir(testdir) if os.path.isfile(os.path.join(testdir, f))]
            self.tests = []
            for test in tests:
                test_cfg = dict()
                test_cfg["filename"] = os.path.join(testdir, test)
                test_cfg["name"] = test
                self.tests.append(Test(test_cfg))

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


class TestOutput(TestResult):
    def __init__(self, out):
        self.out = out

    def __str__(self):
        return self.out


class TestTimeout(TestResult):
    def __str__(self):
        return "TO"


class ExecutionFailureError(RuntimeError):
    def __init__(self, cmd, exitcode):
        super(RuntimeError, self).__init__("Command '{}' failed with exit code {}".format(cmd, exitcode))


def run(config, solution, test):
    with open(test.filename, "r") as test_in:
        tstart = datetime.datetime.now()
        proc = subprocess.Popen([solution.executable],
                                stdin=test_in,
                                stdout=subprocess.PIPE)
        if config.timeout > 0:
            try:
                proc.wait(timeout=config.timeout)
            except subprocess.TimeoutExpired:
                proc.kill()
                return TestOutput("--"), TestTimeout()
        else:
            proc.wait()
        tend = datetime.datetime.now()
        if proc.returncode != 0:
            raise ExecutionFailureError("{} < {}".format(solution.executable, test.filename), proc.returncode)
        return TestOutput(proc.stdout.read().decode('utf-8').rstrip()), TestSuccess((tend - tstart) // datetime.timedelta(microseconds=1000))


def run_all(config):
    data = []
    headers = ["test"]
    for x in config.solutions:
        headers.append(x.name)
        headers.append("")
    for test in config.tests:
        row = [test.name]
        for solution in config.solutions:
            a, b = run(config, solution, test)
            row.append(a)
            row.append(b)
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
