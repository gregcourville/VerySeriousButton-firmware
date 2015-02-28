#!/bin/env python

import sys

if __name__ == "__main__":
	serial = sys.argv[1].strip()
	with open("vsb_serial.h", "w") as of:
		of.write(
			"#pragma once\n"
			+ '#define VSB_SERIAL_NO_STRL L"%s"\n' % (serial,)
			+ '#define VSB_SERIAL_NO "%s"\n' % (serial,)
			+ "#define VSB_SERIAL_NO_LEN %d\n" % (len(serial),)
			)
