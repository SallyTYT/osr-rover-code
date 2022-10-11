#!/usr/bin/env python
#import datetime
import time
import rospy
from osr_msgs.msg import Status
from sensor_msgs.msg import Joy
from screen import LedScreen

SWITCH_FACE_INC = 1			# Button B to swtich face (increment)
SWITCH_FACE_DEC = 2			# Button X to swtich face (decrement)
SWITCH_FACE_PAGE_INC = 3	# Button Y to swtich face page (increment)
SWITCH_FACE_PAGE_DEC = 0	# Button A to swtich face page (decrement)
SWITCH_FACE_MODE = 7		# Button START to swtich face mode - 0, face only mode / 1, debug mode

screen = LedScreen()

def callback_status(data):
	#rospy.loginfo(data)
	#send usb-> ttl serial commands
	screen.build_msg(1,data.battery,data.error_status,data.temp,data.current)
	screen.check_for_afffirm()

	# TEST 20220928 BEGIN
	# thetime = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
	# with open("/home/ubuntu/LEDCallback.log",'a') as f:
	#     f.write(thetime)
	#     f.write("\n")
	# TEST 20220928 END

	# TEST 20220929 BEGIN
	# print data
	# TEST 20220929 END

def callback_joy(data):
	if (data.buttons[SWITCH_FACE_INC] != 0):
		screen.switch_face(SWITCH_FACE_INC)
		
	if (data.buttons[SWITCH_FACE_DEC] != 0):
		screen.switch_face(SWITCH_FACE_DEC)

	if (data.buttons[SWITCH_FACE_PAGE_INC] != 0):
		screen.switch_face(SWITCH_FACE_PAGE_INC)

	if (data.buttons[SWITCH_FACE_PAGE_DEC] != 0):
		screen.switch_face(SWITCH_FACE_PAGE_DEC)

	if (data.buttons[SWITCH_FACE_MODE] != 0):
		screen.switch_face(SWITCH_FACE_MODE)
	
def shutdown():
	screen.transistion_to_idle()
	return 0

if __name__ == "__main__":

	rospy.init_node("led_screen")
	rospy.loginfo("Starting the led_screen node")
	rospy.on_shutdown(shutdown)
	sub_status  = rospy.Subscriber("/status", Status, callback_status)
	sub_joy     = rospy.Subscriber("/joy", Joy, callback_joy)
	rospy.spin()
