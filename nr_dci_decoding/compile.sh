#!/bin/bash

gcc -c *.c -lm -mavx2 && gcc *.o -lm
