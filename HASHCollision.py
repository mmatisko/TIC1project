import hashlib
import logging
import argparse
from pybloomfilter import BloomFilter
# from memory_profiler import profile
import redis


class HASHCollision(object):

    bf = None
    number = None
    length = None
    redis_database = None
    array_access_counter = 0
    bloom_filter_size = 200000000

    def __init__(self, param):

        self.bf = BloomFilter(self.bloom_filter_size, 0.01, param.bloom_file)
        self.length = param.length
        self.number = param.number

        self.redis_database = redis.StrictRedis()

#    @profile
    def findCollision(self):

        n = 0
        pipe = self.redis_database.pipeline()

        logging.debug("Start of collision finding")
        number = self.number

        logging.debug("Start string: " + str(number))
        logging.debug("Length: " + str(self.length))
        logging.debug("Bloom filter size: " + str(self.bloom_filter_size))

        hashed_number = hashlib.sha256(self.number.encode()).hexdigest()[:self.length]
        self.bf.add(hashed_number)

        self.redis_database.set(hashed_number, number)

        while (True):
            number = hashed_number
            hashed_number = hashlib.sha256(number.encode()).hexdigest()[:self.length]

            if (hashed_number in self.bf):
                self.array_access_counter += 1
                logging.debug("Array access counter: " + str(self.array_access_counter))
                logging.debug("Current number of hashes in dict: " + str(self.redis_database.dbsize()))

                pipe.execute()
                pipe = self.redis_database.pipeline()

                if self.redis_database.exists(hashed_number):
                    print("Start number: " + self.redis_database.get(hashed_number) + " Hash: " + hashed_number)
                    print("End number:   " + number + " Hash: " + hashed_number)
                    print("Array access counter: " + str(self.array_access_counter))
                    logging.debug("Total number of hashes in dict: " + str(self.redis_database.dbsize()))
                    break

            self.bf.add(hashed_number)
            pipe.set(hashed_number, number)

            n += 1
            if (n % 100000) == 0:
                n = 0
                pipe.execute()
                pipe = self.redis_database.pipeline()


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

        parser.add_argument("-b", "--bloom_file", help="File for storing Bloom filter",
                            required=False, dest="bloomfile", default="filter.bloom")

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
        self.bloom_file = arguments.bloomfile
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
        logging.debug("KeyboardInterupt")
        logging.debug("Number of hashes in dict: " + str(HC.redis_database.dbsize()))
