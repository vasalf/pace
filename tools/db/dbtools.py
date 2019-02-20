#!/usr/bin/python3

import instance
import json
import os.path


class Database:
    def __init__(self, config):
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

    def update_all(self):
        for test in self.tests:
            test.update_info()
            test.store_info(self.db)

    def write(self):
        with open(self.db_filename, "w") as db:
            json.dump(self.db, db, sort_keys=True, indent=4)
