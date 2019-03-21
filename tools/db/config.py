#!/usr/bin/python3
# *-* coding: utf-8 *-*

import json
import os
import os.path


class SolutionConfig:
    def __init__(self, config, st_config):
        self.executable = st_config["executable"]
        self.name = st_config["name"]
        self.hidden = st_config.get("hidden", False)
        self.external = st_config.get("external", False)



class TestConfig:
    def __init__(self, config):
        self.filename = config["filename"]
        self.name = config["name"]
        self.answer = config["answer"]


class Testset:
    def __init__(self, config, database_dir):
        if "tests" in config.keys():
            for test in config["tests"]:
                test["answer"] = os.path.join(database_dir, test["name"])
            self.tests = list(map(TestConfig, config["tests"]))
        else:
            tests = [f for f in os.listdir(config["testdir"]) if os.path.isfile(os.path.join(config["testdir"], f))]
            self.tests = []
            for test in tests:
                test_cfg = {}
                test_cfg["filename"] = os.path.join(config["testdir"], test)
                test_cfg["name"] = test
                test_cfg["answer"] = os.path.join(database_dir, test)
                self.tests.append(TestConfig(test_cfg))
            self.tests.sort(key=lambda x: x.filename)


class Config:
    def __init__(self, filename):
        with open(filename, "r") as config:
            data = json.load(config)
        with open(data["speedtest_conf"]) as st_config:
            st_data = json.load(st_config)
        self.database_dir = st_data["database_dir"]
        self.solutions = [SolutionConfig(data, solution) for solution in st_data["solutions"]]
        if "testsets" in st_data:
            self.testsets = list(map(lambda x: Testset(x, self.database_dir), st_data["testsets"]))
        else:
            self.testsets = [Testset(st_data, self.database_dir)]
        self.tests = sum(map(lambda x: x.tests, self.testsets), [])

