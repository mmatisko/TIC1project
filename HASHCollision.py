import hashlib
import logging
import argparse
from pybloomfilter import BloomFilter
# from memory_profiler import profile
from collections import defaultdict


class HASHCollision(object):

    bf = None
    number = None
    length = None
    hash_dict = defaultdict(str)
    array_access_counter = 0
    bloom_filter_size = 200000000

    def __init__(self, param):

        self.bf = BloomFilter(self.bloom_filter_size, 0.01, param.bloom_file)
        self.length = param.length
        self.number = param.number

        logging.debug("\n\
                    HASHCollision initialization\n\
                    <DICTIONARY>\n\
                    Bloom filter - storage:" + str(self.bloom_filter_size) + ", error rate: 0.01\n\
                    Hash length (hexa) " + str(self.length))

#    @profile
    def findCollision(self):

        logging.debug("Start of collision finding")
        number = self.number

        hashed_number = hashlib.sha256(self.number).hexdigest()[:self.length]
        self.bf.add(hashed_number)

        self.hash_dict[hashed_number] = number

        while (True):
            number = hashed_number
            hashed_number = hashlib.sha256(number).hexdigest()[:self.length]

            if (hashed_number in self.bf):
                self.array_access_counter += 1
                logging.debug("\n\
                    Array access counter: " + str(self.array_access_counter))

                if (hashed_number in self.hash_dict):
                    print("First  number: " + self.hash_dict[hashed_number] + " Hash: " + hashed_number)
                    print("Second number: " + number + " Hash: " + hashed_number)

                    logging.debug("\n\
                    Search finished\n\
                    First  number: " + self.hash_dict[hashed_number] + " Hash: " + hashed_number + "\n\
                    Second number: " + number + " Hash: " + hashed_number + "\n\
                    Array access counter: " + str(self.array_access_counter))

                    break

            self.bf.add(hashed_number)
            self.hash_dict[hashed_number] = number


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
        logging.debug("\n\
                    KeyboardInterupt\n\
                    Number of hashes in dict: " + str(HC.redis_database.dbsize()))
