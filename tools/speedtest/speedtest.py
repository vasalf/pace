#!/usr/bin/python3
# *-* coding: utf-8 *-*


import argparse
import abc
import concurrent.futures
import datetime
import functools
import json
import os
import os.path
import re
import resource
import subprocess
import sys
import tabulate
import tempfile
import threading


class Test:
    def __init__(self, config):
        self.filename = config["filename"]
        self.name = config.get("name", self.filename)


class Solution:
    def __init__(self, config):
        self.executable = config["executable"]
        self.name = config.get("name", self.executable)
        self.hidden = config.get("hidden", False)
        self.external = config.get("external", False)
        self.forced = False


VC_SOLUTION = re.compile(".*s vc \\d+ (\\d+).*")


class TestResultDatabase:
    def __init__(self, directory, externals):
        self.directory = directory
        self.externals = externals
        self.lock = threading.Lock()

    def __cur_solution(self, test):
        sf = os.path.join(self.directory, test.name)
        if os.path.isfile(sf):
            with open(sf, "r") as f:
                s = f.readline()
                while s != "":
                    m = VC_SOLUTION.match(s)
                    if m is not None:
                        return int(m.groups()[0])
                    s = f.readline()
        return None

    def __is_externally_solved(self, test):
        sf = os.path.join(self.directory, test.name)
        if os.path.isfile(sf):
            with open(sf, "r") as f:
                s = f.readline()
            solution = s.split()[1]
            return solution in self.externals
        return False

    def __new_solution(self, result):
        m = VC_SOLUTION.match(result)
        if m is not None:
            return int(m.groups()[0])
        return None

    def __write_solution(self, test, solution, result):
        sf = os.path.join(self.directory, test.name)
        with open(sf, "w") as f:
            f.write("c {}\n".format(solution.name))
            f.write(result)

    def process_result(self, test, solution, result):
        with self.lock:
            cs = self.__cur_solution(test)
            ns = self.__new_solution(result.replace("\n", "#"))
            if cs is None:
                self.__write_solution(test, solution, result)
                return 1
            elif ns != cs:
                return -1
            elif solution not in self.externals and self.__is_externally_solved(test):
                self.__write_solution(test, solution, result)
                return 1
        return 0


class Config:
    def __init__(self, args, data):
        self.timeout = data.get("timeout", -1)
        self.output = data.get("output", "plain")
        self.memory_limit = data.get("memory_limit", 1024 ** 3)
        self.validate = data.get("validate", None)
        self.threads = args.threads

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
            self.tests.sort(key=lambda x: x.filename)

        if "database_dir" in data:
            self.database_dir = data["database_dir"]
        else:
            self.database_dir = None

        self.solutions = list(map(Solution, data["solutions"]))

        if args.filter_tests != "":
            filter_tests = args.filter_tests.split(",")
            self.tests = list(filter(lambda x: x.name in filter_tests, self.tests))

        if args.filter_solutions != "":
            filter_solutions = args.filter_solutions.split(",")
            self.solutions = list(filter(lambda x: x.name in filter_solutions, self.solutions))
            for solution in self.solutions:
                solution.forced = True
            self.unforced_solutions = self.solutions
        else:
            self.unforced_solutions = list(filter(lambda x: not x.hidden, self.solutions))

        if args.timeout > 0:
            self.timeout = args.timeout

        if args.memory_limit > 0:
            self.memory_limit = args.memory_limit


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


NUMBER_OF_VERTICES = re.compile(".*p td (\\d+).*")


class TestOutput(TestResult):
    def __init__(self, test, out):
        self.test = test
        self.out = out

    def __str__(self):
        with open(self.test.filename, 'r') as f:
            m = NUMBER_OF_VERTICES.match(f.read().replace("\n", "#"))
            if m is None:
                n = "--"
            else:
                n = m.groups()[0]
        return '{}/{}'.format(self.out, n)


class VCTestOutput(TestResult):
    def __init__(self, test, out):
        self.sz = VC_SOLUTION.match(out.replace("\n", "#"))
        if self.sz is None:
            self.sz = "--"
        else:
            self.sz = self.sz.groups()[0]

    def __str__(self):
        return self.sz


def make_test_output(method, test, out):
    if method == "plain":
        return TestOutput(test, out)
    if method == "vc":
        return VCTestOutput(test, out)

    raise RuntimeException("Unknown output method {}".format(method))


class TestTimeout(TestResult):
    def __str__(self):
        return "TO"


class TestCrash(TestResult):
    def __str__(self):
        return "CR"


class TestValidationFailure(TestResult):
    def __str__(self):
        return "VF"


class ExecutionFailureError(RuntimeError):
    def __init__(self, cmd, exitcode, results):
        self.results = results
        super(RuntimeError, self).__init__("Command '{}' failed with exit code {}".format(cmd, exitcode))


class ValidationError(RuntimeError):
    def __init__(self, cmd, results):
        self.results = results
        super(RuntimeError, self).__init__("Command '{}' did not pass the validation".format(cmd))


def validate_output(config, test, output):
    proc = subprocess.Popen([config.validate, "--graph", test.filename], stdin=subprocess.PIPE)
    proc.communicate(input=output.encode("utf8"))
    proc.wait()
    return proc.returncode == 0


def run(config, database, solution, test):
    def set_memory_limit(limit):
        resource.setrlimit(resource.RLIMIT_AS, (limit, resource.RLIM_INFINITY))

    test_output = functools.partial(make_test_output, config.output)
    with open(test.filename, "r") as test_in:
        with tempfile.TemporaryFile() as test_out:
            tstart = datetime.datetime.now()
            proc = subprocess.Popen([solution.executable],
                                    stdin=test_in,
                                    stdout=test_out,
                                    preexec_fn=functools.partial(set_memory_limit, config.memory_limit))
            if config.timeout > 0:
                try:
                    proc.wait(timeout=config.timeout)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    return test_output(test, "--"), TestTimeout(), 0
            else:
                proc.wait()
            tend = datetime.datetime.now()
            if proc.returncode != 0:
                a, b = test_output(test, "--"), TestCrash()
                raise ExecutionFailureError("{} < {}".format(solution.executable, test.filename), proc.returncode, (a, b, 0))

            test_out.seek(0)
            output = test_out.read().decode('utf-8').rstrip()
            if config.validate is not None:
                if not validate_output(config, test, output):
                    a, b = test_output(test, "--"), TestValidationFailure()
                    raise ValidationError("{} < {}".format(solution.executable, test.filename), (a, b, 0))
            if database is not None:
                dbres = database.process_result(test, solution, output)
            else:
                dbres = 0

            return test_output(test, output), TestSuccess((tend - tstart) // datetime.timedelta(microseconds=1000)), dbres


class SpeedtestExecutor:
    def __init__(self, config):
        self.config = config
        self.err_msgs = []
        self.succ_msgs = []

        if config.database_dir is not None:
            externals = set()
            for solution in config.solutions:
                if solution.external:
                    externals.add(solution.name)
            self.database = TestResultDatabase(config.database_dir, externals)
        else:
            self.database = None

    def launch(self):
        self.executor = concurrent.futures.ThreadPoolExecutor(max_workers=self.config.threads)
        self.futures = []
        for test in self.config.tests:
            futures = []
            for solution in self.config.unforced_solutions:
                futures.append(self.executor.submit(run, self.config, self.database, solution, test))
            self.futures.append(futures)

    def finish(self):
        data = []
        headers = ["test"]
        for x in self.config.solutions:
            headers.append(x.name)
            headers.append("")
        for i, test in enumerate(self.config.tests):
            row = [test.name]
            for j, solution in enumerate(self.config.unforced_solutions):
                try:
                    a, b, dbres = self.futures[i][j].result()
                except ExecutionFailureError as e:
                    a, b, dbres = e.results
                    self.err_msgs.append("EXECUTION FAILURE: {}".format(str(e)))
                except ValidationError as e:
                    a, b, dbres = e.results
                    self.err_msgs.append("VALIDATION FAILURE: {}".format(str(e)))
                row.append(a)
                row.append(b)
                if dbres == 1:
                    self.succ_msgs.append("SUCCESS: Found solution for test {}".format(test.name))
                elif dbres == -1:
                    self.err_msgs.append("SUSPECT: Found solution of another size for test {}".format(test.name))
            data.append(row)
        self.table = tabulate.tabulate(data, headers=headers)

    def result(self):
        return "\n".join([self.table] + self.succ_msgs + self.err_msgs)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('config',
                        metavar="CONFIG",
                        type=str,
                        help='Path to configuration file')
    parser.add_argument('--threads',
                        metavar="THREADS",
                        type=int,
                        default=1,
                        help='Number of execution threads (DEFAULT: 1)')
    parser.add_argument('--filter-tests',
                        type=str,
                        default="",
                        help='Comma-separated list of names of tests to be launched (DEFAULT: launch all)')
    parser.add_argument('--filter-solutions',
                        type=str,
                        default="",
                        help='Comma-separated list of names of solutions to be launched (DEFAULT: launch all)')
    parser.add_argument('--timeout',
                        type=int,
                        default=0,
                        help='When non-zero, overrides timeout in seconds from config (DEFAULT: 0)')
    parser.add_argument('--memory-limit',
                        type=int,
                        default=0,
                        help='When non-zero, overrides memory limit in bytes from config (DEFAULT: 0)')
    args = parser.parse_args()

    with open(args.config, 'r') as conf:
        config = Config(args, json.load(conf))

    executor = SpeedtestExecutor(config)
    executor.launch()
    executor.finish()

    print(executor.result())


if __name__ == "__main__":
    main()
