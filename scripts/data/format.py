#!/usr/bin/env python3

import argparse
import sys

COL_COUNT = 1 + 2 * 3 * 3 * 3


def printw(s: str, w: int) -> None:
    print(end=s)
    print(end=' ' * (w - len(s)))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('filename')

    args = parser.parse_args()

    with open(args.filename) as file:
        lines = file.readlines()

    lines = [line.split() for line in lines]
    lens = [5] * COL_COUNT

    for line in lines:
        if len(line) != COL_COUNT:
            print('Invalid line length', file=sys.stderr)
            sys.exit(1)

        for i in range(COL_COUNT):
            lens[i] = max(len(line[i]) + 1, lens[i])

    j = 0
    printw('zmax', lens[j])

    for t in ('b', 'm', 't'):
        for m in ('b', 'm', 't'):
            for r in ('b', 'm', 't'):
                for p in ('2', '3'):
                    j += 1
                    printw(f'{t}{m}{r}{p}', lens[j])
    print()

    for line in lines:
        for i in range(COL_COUNT):
            printw(line[i].replace(',', '.'), lens[i])
        print()


if __name__ == '__main__':
    main()
