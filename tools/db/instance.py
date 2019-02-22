#!/usr/bin/python3
# *-* coding: utf-8 *-*

import os.path
import subprocess
import tempfile


class TestInfo:
    UPDATE_OPS = [
        "update_graph_stats",
        "update_solution",
        "update_surplus",
        "update_lpvc",
        "update_greed",
        "update_crbound",
        "update_kernel_stats",
    ]

    def __init__(self, config, db=None):
        self.config = config
        if db is None:
            self.info = {}
        else:
            self.info = db

    def update(self):
        for op in TestInfo.UPDATE_OPS:
            getattr(self, op)()

    def store(self):
        return self.info

    def __getitem__(self, option):
        return self.info[option]

    def get(self, *args, **kwargs):
        return self.info.get(*args, **kwargs)

    def update_graph_stats(self):
        if "n" in self.info and "m" in self.info and "mind" in self.info and "maxd" in self.info:
            return
        with open(self.config.filename, "r") as test:
            p = subprocess.Popen(["./build/tools/graph_stats/graph_stats"], stdin=test, stdout=subprocess.PIPE)
            p.wait()
            nstr = p.stdout.readline().decode("utf-8").rstrip()
            mstr = p.stdout.readline().decode("utf-8").rstrip()
            mndstr = p.stdout.readline().decode("utf-8").rstrip()
            mxdstr = p.stdout.readline().decode("utf-8").rstrip()
        self.info["n"] = int(nstr.split()[1])
        self.info["m"] = int(mstr.split()[1])
        self.info["mnd"] = int(mndstr.split()[1])
        self.info["mxd"] = int(mxdstr.split()[1])

    def update_solution(self):
        solution_filename = self.config.answer
        size = None
        solution = None
        if os.path.exists(solution_filename):
            with open(solution_filename, "r") as answer:
                solution = answer.readline().split()[1]
                s = answer.readline()
                while s[0] == 'c':
                    s = answer.readline()
                size = int(s.split()[-1])
        if solution is not None:
            if solution != self.info.get("solved_by", ""):
                self.info["solved_by"] = solution
                self.info["answer"] = size

    def update_surplus(self):
        if "surplus" in self.info:
            return
        with tempfile.TemporaryFile() as out:
            with open(self.config.filename, "r") as test:
                p = subprocess.Popen(["./build/tools/surplus/surplus"], stdin=test, stdout=out)
                p.wait()
            out.seek(0)
            surlus_str = out.readline().decode("utf-8")
        self.info["surplus"] = int(surlus_str.split("=")[1])

    def update_lpvc(self):
        if "lpvc" in self.info:
            return
        with open(self.config.filename, "r") as test:
            p = subprocess.Popen(["./build/tools/lpvc/lpvc"], stdin=test, stdout=subprocess.PIPE)
            p.wait()
            self.info["lpvc"] = float(p.stdout.read().decode("utf-8").rstrip())

    def update_greed(self):
        if "greed" in self.info:
            return
        with open(self.config.filename, "r") as test:
            p = subprocess.Popen(["./build/tools/greed/greed"], stdin=test, stdout=subprocess.PIPE)
            p.wait()
            self.info["greed"] = int(p.stdout.read().decode("utf-8").rstrip())

    def update_crbound(self):
        if "crbound" in self.info:
            return
        with open(self.config.filename, "r") as test:
            p = subprocess.Popen(["./build/tools/crown_bound/crown_bound"], stdin=test, stdout=subprocess.PIPE)
            p.wait()
            self.info["crbound"] = int(p.stdout.read().decode("utf-8").rstrip())

    def __update_kernel_stats(self, which):
        if which in self.info:
            return
        with open(self.config.filename, "r") as test:
            p = subprocess.Popen([f"./build/tools/kernel_stats/{which}/{which}"], stdin=test, stdout=subprocess.PIPE)
            p.wait()
            nstr = p.stdout.readline().decode("utf-8").rstrip()
            mstr = p.stdout.readline().decode("utf-8").rstrip()
            compstr = p.stdout.readline().decode("utf-8").rstrip()
        self.info[which] = {
            "n": int(nstr.split()[1]),
            "m": int(mstr.split()[1]),
            "comps": " ".join(compstr.split()[1:])
        }

    def update_kernel_stats(self):
        self.__update_kernel_stats("stat_zs")
        self.__update_kernel_stats("stat_c2k")


class Test:
    def __init__(self, config):
        self.config = config
        self.info = TestInfo(config)

    def load_info(self, db):
        self.info = TestInfo(self.config, db[self.config.name])

    def update_info(self):
        self.info.update()

    def store_info(self, db):
        db[self.config.name] = self.info.store()
