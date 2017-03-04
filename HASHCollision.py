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
    bloom_filter_size = 3000000000
    hash_length = None
    batch_size = 100000

    def __init__(self, param):

        self.bf = BloomFilter(self.bloom_filter_size, 0.01)
        self.length = param.length
        self.number = param.number

        self.redis_database = redis.StrictRedis(
                host='localhost', port=6379)

        self.hash_length = self.length//2
        logging.debug("\n\
                    HASHCollision initialization\n\
                    Redis database - localhost:6379\n\
                    <internal implementation>\n\
                    Bloom filter - storage:" + str(self.bloom_filter_size) + ", error rate: 0.01\n\
                    Hash length (hexa) " + str(self.length) + "\n\
                    HSET hash length: " + str(self.hash_length) + "\n\
                    Batch size: " + str(self.batch_size))

#    @profile
    def findCollision(self):

        n = 0
        pipe = self.redis_database.pipeline()

        logging.debug("Start of collision finding")
        number = self.number

        hashed_number = hashlib.sha256(self.number.encode()).hexdigest()[:self.length]
        self.bf.add(hashed_number)

        self.redis_database.hset(hashed_number[:self.hash_length], hashed_number, number)

        while (True):
            number = hashed_number
            hashed_number = hashlib.sha256(number.encode()).hexdigest()[:self.length]

            if (hashed_number in self.bf):
                self.array_access_counter += 1
                logging.debug("\n\
                    Array access counter: " + str(self.array_access_counter))

                pipe.execute()
                pipe = self.redis_database.pipeline()

                if self.redis_database.hexists(hashed_number[:self.hash_length], hashed_number):
                    print("First  number: " + self.redis_database.hget(hashed_number[:self.hash_length], hashed_number).decode('utf-8') + " Hash: " + hashed_number)
                    print("Second number: " + number + " Hash: " + hashed_number)

                    size = int(self.redis_database.info()['used_memory'])
                    print('Database size: {} bytes, {} MB'.format(size, size / 1024 / 1024))

                    logging.debug("\n\
                    Search finished\n\
                    First  number: " + self.redis_database.hget(hashed_number[:self.hash_length], hashed_number).decode('utf-8') + " Hash: " + hashed_number + "\n\
                    Second number: " + number + " Hash: " + hashed_number + "\n\
                    Array access counter: " + str(self.array_access_counter) + "\n\
                    Number of hashes in dict: " + str(self.redis_database.dbsize()) + "\n\
                    Clear database")

                    self.redis_database.flushall()
                    break

            self.bf.add(hashed_number)
            batch = hashed_number[:self.hash_length]
            pipe.hset(batch, hashed_number, number)

            n += 1
            if (n % self.batch_size) == 0:
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
                    Number of hashes in dict: " + str(HC.redis_database.dbsize()))
        HC.redis_database.flushall()
