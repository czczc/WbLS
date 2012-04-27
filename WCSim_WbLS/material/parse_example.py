#!/usr/bin/env python

import json

# decode
WbLS = json.loads(open('WbLS.json').read())
print WbLS

# dump new json data
WbLS['SCINTILLATIONYIELD']['scaling'] = 100
print json.dumps(WbLS, indent=2)
