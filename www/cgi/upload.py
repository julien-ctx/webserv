#!/usr/local/bin/python3
import sys
import os

if __name__ == "__main__":
	stdin = ""
	for line in sys.stdin:
		stdin += line
	boundary = stdin.split('\n')[0]
	stdin = stdin.split(boundary)
	content = ""
	msg = "The file has been successfully uploaded"

	for block in stdin:
		if "filename=" in block:
			split = block.split("\r\n")
			content = ""
			filename = ""
			content = split[-2]
			for line in split:
				if "filename=" in line:
					filename = line.split('filename="', 1)[1].split('"')[0]
			if content and filename:
				if (os.path.isfile("./www/cgi/uploads/" + filename)):
					msg = "A file with the same name already exists!"
				with open("./www/cgi/uploads/" + filename, 'w') as file:
					file.write(content)
			else:
				msg = "Filename or file data is invalid!"

	print(msg)
