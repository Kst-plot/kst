#!/usr/bin/env python
import sys
for path in sys.path[:]:
    if path.rfind('-packages') != -1:
        print path
        exit()
