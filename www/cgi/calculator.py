#!/usr/local/bin/python3
import sys
import math

if __name__ == "__main__":
	if len(sys.argv) == 4:
		try:
			if sys.argv[2] == '*':
				print(int(sys.argv[1]) * int(sys.argv[3]))
			elif sys.argv[2] == '-':
				print(int(sys.argv[1]) - int(sys.argv[3]))
			elif sys.argv[2] == '+':
				print(int(sys.argv[1]) + int(sys.argv[3]))
			elif sys.argv[2] == '/':
				if sys.argv[3] == '0':
					sys.exit("Error")
				print(round(float(int(sys.argv[1]) / int(sys.argv[3])), 4))
		except Exception:
			print("Error")
	else:
		print("Error")
