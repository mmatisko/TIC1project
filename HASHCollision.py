import hashlib
import logging
import argparse
from pybloomfilter import BloomFilter
import lmdb
# from memory_profiler import profile


class HASHCollision(object):

    bf = None
    number = None
    length = None
    lmdb_database = None
    array_access_counter = 0
    bloom_filter_size = 3000000000

    def __init__(self, param):

        self.bf = BloomFilter(self.bloom_filter_size, 0.01)
        self.length = param.length
        self.number = param.number

        self.lmdb_database = lmdb.open("HASHCollision_db", writemap=True, sync=False, map_size=4294967296)

        logging.debug("\n\
                    HASHCollision initialization\n\
                    LMDB database - HASHCollision_db\n\
                    Bloom filter - storage:" + str(self.bloom_filter_size) + ", error rate: 0.01\n\
                    Hash length (hexa) " + str(self.length))

    # @profile
    def findCollision(self):

        logging.debug("Start of collision finding")
        number = self.number

        hashed_number = hashlib.sha256(self.number.encode()).hexdigest()[:self.length]
        self.bf.add(hashed_number)

        with self.lmdb_database.begin(write=True) as db:
            db.put(hashed_number.encode(), number.encode())

        while (True):
            number = hashed_number
            hashed_number = hashlib.sha256(number.encode()).hexdigest()[:self.length]

            if (hashed_number in self.bf):
                self.array_access_counter += 1
                logging.debug("\n\
                Array access counter: " + str(self.array_access_counter))

                with self.lmdb_database.begin() as db:
                    if db.get(hashed_number.encode()) is not None:
                        print("First  number: " + db.get(hashed_number.encode()).decode() + " Hash: " + hashed_number)
                        print("Second number: " + number + " Hash: " + hashed_number)

                        print(self.lmdb_database.stat())

                        logging.debug("\n\
                    Search finished\n\
                    First  number: " + db.get(hashed_number.encode()).decode() + " Hash: " + hashed_number + "\n\
                    Second number: " + number + " Hash: " + hashed_number + "\n\
                    Array access counter: " + str(self.array_access_counter) + "\n\
                    DB Stat: " + str(self.lmdb_database.stat()))

                        break

            self.bf.add(hashed_number)

            with self.lmdb_database.begin(write=True) as db:
                db.put(hashed_number.encode(), number.encode())


class ParseParams(object):

    number = None
    length = None
    log_file = None
    bloom_file = None

    def parseParams(self):
        parser = argparse.ArgumentParser(description='''
        HASHColision
        Script for detecting SHA 256 colisions on parametr set bit length.
        ''')
        parser.add_argument("-c", "--length", help="Lenght of hash used for comparision",
                            required=True, dest="length", type=int,
                            default=3)

        parser.add_argument("-n", "--number", help="First number for hash",
                            required=False, dest="number", default="RANDOM")

        parser.add_argument("--log_file", help="Log file name",
                            required=False, dest="logfile", default="HASHCollision.log")

        parser.add_argument("-l", help="Enable logging", action="store_true",
                            required=False, dest="logging", default=False)

        arguments = parser.parse_args()

        if arguments.number is "RANDOM":
            self.number = "7416890346"  # value chosen by girlfriend
        else:
            self.number = str(arguments.number)

        self.length = arguments.length
        self.log_file = arguments.logfile

        if arguments.logging:
            logging.basicConfig(filename=param.log_file, level=logging.DEBUG, filemode='w', format='%(asctime)s %(message)s')


if __name__ == "__main__":
    param = ParseParams()
    param.parseParams()

    HC = HASHCollision(param)
    try:
        HC.findCollision()
    except KeyboardInterrupt:
        logging.debug("\n\
                    KeyboardInterupt\n\
                    DB stat: " + str(HC.lmdb_database.stat()))
        print(HC.lmdb_database.stat())
        # HC.lmdb_database.close()
