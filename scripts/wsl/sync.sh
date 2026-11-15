#!/bin/bash
# scripts/wsl/sync.sh -- 把仓库同步到 WSL 家目录副本（~/CarbonUI_app），排除构建产物
set -eu
. "$(dirname -- "$0")/lib.sh"
wsl_sync
