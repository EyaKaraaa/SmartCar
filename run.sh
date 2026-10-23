#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")"
output_binary="$(mktemp /tmp/smartcar-test.XXXXXX)"
trap 'rm -f "$output_binary"' EXIT
g++ -std=c++17 -Wall -Wextra -Werror -DCONFIG_BT_ENABLED -DCONFIG_BLUEDROID_ENABLED -DCONFIG_BT_SPP_ENABLED -I stubs control_test.cpp -o "$output_binary"
"$output_binary"
