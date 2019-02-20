#!/usr/bin/python3
# *-* coding: utf-8 *-*


import jinja2


class TestRepr:
    def __init__(self, test):
        self.name = test.config.name
        self.n = test.info["n"]
        self.m = test.info["m"]


def render(database):
    tests = list(map(TestRepr, database.tests))
    env = jinja2.Environment(
        loader=jinja2.FileSystemLoader("tools/db/templates"),
        autoescape=jinja2.select_autoescape(['html', 'xml'])
    )
    template = env.get_template("tests.html")
    with open(database.render_path, "w") as res:
        res.write(template.render(tests=tests))
