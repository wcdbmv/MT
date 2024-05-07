#!/usr/bin/env bash

set -euxo pipefail

git ls-files | grep -e "\.h$" -e "\.cc$" | xargs clang-format -i
