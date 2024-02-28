#!/bin/bash
#Author:HuangShiqing
#Time:2020-11-03 20:06:09
#Name:mk.sh
#Description:This is a awesome script.

CROSS_COMPILE= riscv64-unknown-linux-gnu-gcc
DIR="./Debug/"
SRC="./src/"

if [ ! -d "${DIR}" ]; then
    mkdir "${DIR}"
fi

${CROSS_COMPILE} "${SRC}"winograd1_sample.c -o winograd1_sample -Ofast
${CROSS_COMPILE} "${SRC}"winograd4_sample.c -o winograd4_sample -Ofast
${CROSS_COMPILE} "${SRC}"winograd5_sample.c -o winograd5_sample -Ofa
${CROSS_COMPILE} -c "${SRC}"common.c -o "${DIR}"common.o -Ofast
${CROSS_COMPILE} -c "${SRC}"conv0.c -o "${DIR}"conv0.o -Ofast
${CROSS_COMPILE} -c "${SRC}"winograd1.c -o "${DIR}"winograd1.o -Ofast
${CROSS_COMPILE} -c "${SRC}"winograd2.c -o "${DIR}"winograd2.o -Ofast
${CROSS_COMPILE} -c "${SRC}"winograd3.c -o "${DIR}"winograd3.o -Ofast
${CROSS_COMPILE} -c "${SRC}"winograd4.c -o "${DIR}"winograd4.o -Ofast
${CROSS_COMPILE} -c "${SRC}"winograd5.c -o "${DIR}"winograd5.o -Ofast
${CROSS_COMPILE} -c "${SRC}"main.c -o "${DIR}"main.o -Ofast
${CROSS_COMPILE} "${DIR}"main.o "${DIR}"common.o "${DIR}"conv0.o "${DIR}"winograd1.o "${DIR}"winograd2.o "${DIR}"winograd3.o "${DIR}"winograd4.o "${DIR}"winograd5.o -o main -Ofast
