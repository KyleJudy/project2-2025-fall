#!/bin/bash

# Requires `parallel`, because I'm lazy.

make -s

printf "%02d\n" {1..10} | \
   parallel -n 1 \
  './main < test_cases/test{}.in 2>/dev/null > /tmp/test{}.out && \
   diff /tmp/test{}.out test_cases/test{}.out || echo "Test {} ^^^ Failed"'
