#!/usr/bin/python3
# *-* coding: utf-8 *-*


class TestInfo:
    UPDATE_OPS = [
        "update_nm",
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


    def update_nm(self):
        with open(self.config.filename, "r") as test:
            s = test.readline()
            while s != "":
                if s[0] != 'c':
                    p, td, n, m = s.split()
                    self.info["n"] = int(n)
                    self.info["m"] = int(m)
                    return
                s = test.readline()


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
