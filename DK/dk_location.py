#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
dk.py: 
observe vehicle attribute (position) changes.

usage

lancio il simulatore quad
dronekit-sitl copter-3.3 --home=46.492248,11.3465918,256,353 --model=quad

lancio mavproxy con due porte qgrouncontrol 144550 e dronekit app 14551 
mavproxy.py --master tcp:127.0.0.1:5760  --out 127.0.0.1:14550 --out 127.0.0.1:14551

cd /Users/lucasoprana/Documents/VScode-project/home-32/DK
python dk_location.py --connect udp:127.0.0.1:14551



"""
from __future__ import print_function
from dronekit import connect, VehicleMode
import time

#Set up option parsing to get connection string
import argparse  
parser = argparse.ArgumentParser(description='Print out vehicle state information. Connects to SITL on local PC by default.')
parser.add_argument('--connect', 
                   help="vehicle connection target string. If not specified, SITL automatically started and used.")
args = parser.parse_args()

connection_string = args.connect
sitl = None


#Start SITL if no connection string specified
if not connection_string:
    import dronekit_sitl
    sitl = dronekit_sitl.start_default()
    connection_string = sitl.connection_string()


# Connect to the Vehicle. 
#   Set `wait_ready=True` to ensure default attributes are populated before `connect()` returns.
print("\nConnecting to vehicle on: %s" % connection_string)
vehicle = connect(connection_string, wait_ready=True)

vehicle.wait_ready('autopilot_version')

print(" Global Location: %s" % vehicle.location.global_frame)
print(" Global Location (relative altitude): %s" % vehicle.location.global_relative_frame)
print(" Local Location: %s" % vehicle.location.local_frame)
print(" Attitude: %s" % vehicle.attitude)
print(" Velocity: %s" % vehicle.velocity)
print(" GPS: %s" % vehicle.gps_0)

#Callback to print the location in global frames. 'value' is the updated value
def location_callback(self, attr_name, value):
    print ("Location (Global): ", value)


# Add a callback `location_callback` for the `global_frame` attribute.
vehicle.add_attribute_listener('location.global_frame', location_callback)

# Wait 2s so callback can be notified before the observer is removed
time.sleep(600)

"""
import os
import time
import sys

#sys.exit()

iname=1
while True:
	dummy=raw_input()
	myname=str(iname)+".jpg"
	os.system("raspistill -t 1000 -sh -100 -ex sports -awb off -q 100 -o /home/pi/"+myname)
	#time.sleep(1.5)
	iname+=1

"""

