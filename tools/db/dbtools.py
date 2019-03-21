#!/usr/bin/python3

import functools
import instance
import json
import os.path
import multiprocessing as mp


class Database:
    def __init__(self, config):
        self.config = config
        self.tests = list(map(instance.Test, config.tests))
        db_filename = os.path.join(config.database_dir, "db.json")
        self.db_filename = db_filename
        self.render_path = os.path.join(config.database_dir, "tests.html")
        if os.path.exists(db_filename):
            with open(db_filename, "r") as db:
                self.db = json.load(db)
        else:
            self.db = {}
            for test in self.tests:
                self.db[test.config.name] = {}
        for test in self.tests:
            test.load_info(self.db)

    @staticmethod
    def do_update(test):
        test.update_info()
        return test 

    def update_all(self, threads=1):
        pool = mp.Pool(threads)
        self.tests = list(pool.map(Database.do_update, self.tests))
        for test in self.tests:
            test.store_info(self.db)


    def write(self):
        with open(self.db_filename, "w") as db:
            json.dump(self.db, db, sort_keys=True, indent=4)
