#!/usr/bin/env python3

import re
import sys


def main() -> None:
    s = sys.stdin.read()

    s = re.sub(r'%.*\n?', '', s)
    s = re.sub(r'~', ' ', s)
    s = re.sub(r'\\ref{[\w:-]+}', '1', s)
    s = re.sub(r'\s*\\(begin|end|def|let).*', '', s)
    s = re.sub(r'\s*\\item', '—', s)
    s = re.sub(r'\\\w+{?|}', '', s)
    s = re.sub(r'\n\n+', '\n\n', s)

    print(s)


if __name__ == '__main__':
    main()
