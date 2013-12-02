"""
Copyright Steinwurf ApS 2011-2013.
Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
See accompanying file LICENSE.rst or
http://www.steinwurf.com/licensing
"""

import pandas
assert pandas.version.version.split(".")[:2] >= ['1','12'],\
    'You need a newer version of pandas'

import pymongo
assert pymongo.version_tuple[:2] >= (2,5),\
    'You need a newer version of pymongo'
from pymongo import MongoClient

from datetime import datetime, timedelta
now = datetime.utcnow()
today = now.date()
today = datetime(today.year, today.month, today.day)
yesterday = today - timedelta(1)

class JsonFile(object):
    """docstring for JsonFile"""
    def __init__(self):
        super(JsonFile, self).__init__()
    def add_options(self, parser):
        parser.add_argument('--jsonfile',
            action  = 'store',
            default = '',
            help    = 'The .json file generated by the benchmark, if none '
                      'provided plots will be based on data from the database.'
        )

    def get_data(self, options):
        if options['jsonfile']:
            df = pandas.read_json(options['jsonfile'])
            return df
        else:
            return None

class MongoDbDatabaseQuery(object):
    database = 'benchmark'
    address = '176.28.49.184'
    username = 'guest'
    password = 'none'
    """docstring for MongoDbDatabaseQuery"""
    def __init__(self, collection, query = None):
        super(MongoDbDatabaseQuery, self).__init__()
        self.collection = collection
        if not query:
            query = {
                'branch'    : 'master',
                'scheduler' : 'kodo-nightly-benchmark',
                'utc_date'  : {'$gte': yesterday}
            }
        self.query = query
    def add_options(self, parser):
        pass

    def __connect():
        """
        Connect to the database
        """
        client = MongoClient(MongoDbDatabaseQuery.address)
        db = client[MongoDbDatabaseQuery.database]
        db.authenticate(MongoDbDatabaseQuery.username,
                        MongoDbDatabaseQuery.password)
        return db

    def get_data(self, options):
        db = __connect()
        collection = list(db[self.collection].find(self.query))
        data = pandas.DataFrame.from_records( collection )
        return data