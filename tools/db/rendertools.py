#!/usr/bin/python3
# *-* coding: utf-8 *-*


import enum
import jinja2


class TestStatus(enum.Enum):
    UNSOLVED = 0,
    EXTERNALLY_SOLVED = 1,
    SOLVED = 2


class KernelStatsRepr:
    def __init__(self, stats):
        self.n = stats["n"]
        self.m = stats["m"]
        self.comps = stats["comps"]


class TestRepr:
    def __init__(self, test):
        self.name = test.config.name
        self.n = test.info["n"]
        self.m = test.info["m"]
        self.mnd = test.info["mnd"]
        self.mxd = test.info["mxd"]
        self.surplus = test.info["surplus"]
        self.lpvc = test.info["lpvc"]
        self.crbound = test.info["crbound"]
        self.greed = test.info["greed"]
        self.answer = test.info.get("answer", "—")
        self.solved_by = test.info.get("solved_by", "—")
        self.zs = KernelStatsRepr(test.info["stat_zs"])
        self.c2k = KernelStatsRepr(test.info["stat_c2k"])

    def update_status(self, database):
        self.status = TestStatus.UNSOLVED
        for solution in database.config.solutions:
            if solution.name == self.solved_by:
                if solution.external:
                    self.status = TestStatus.EXTERNALLY_SOLVED
                else:
                    self.status = TestStatus.SOLVED
        self.status = str(self.status)


def render(database):
    tests = list(map(TestRepr, database.tests))
    for test in tests:
        test.update_status(database)
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader("tools/db/templates"),
        autoescape=jinja2.select_autoescape(['html', 'xml'])
    )
    template = env.get_template("tests.html")
    with open(database.render_path, "w") as res:
        res.write(template.render(tests=tests))
