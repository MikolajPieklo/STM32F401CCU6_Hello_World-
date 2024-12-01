#!/usr/bin/env python3

import argparse
import os

def print_size():
   status = os.path.isfile(args.source)

   if False == status:
      print("File not exist!")
      return -1

   size = os.path.getsize(args.source)
   size = int(size)
   print("Size: " + str(size))

   usage = ((100 * size) / 256000)
   print("Usage: " + str(usage) + "%")

def parse_args():
   global args

   parser = argparse.ArgumentParser(
      description="Print size of bin file",
      formatter_class=argparse.RawTextHelpFormatter
   )

   parser.add_argument(
      "source", help="source file"
   )

   args = parser.parse_args()


if __name__ == '__main__':
   parse_args()

   print_size()