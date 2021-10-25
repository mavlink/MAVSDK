#!/usr/bin/env python3
import sys

"""
Dummy winch control script
"""

def winch_control(signal):
    if (signal == "descend"):
        print("\n - Descending payload...\n");
    elif (signal == "raise"):
        print("\n - Raising winch...\n");

def main():
    winch_control(sys.argv[1])


main()
