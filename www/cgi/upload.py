#!/usr/local/bin/python3
import sys
import os

if __name__ == "__main__":
	stdin = b""
	for line in sys.stdin.buffer:
		stdin += line
	if not stdin:
		exit()
	boundary = stdin.split(b'\n')[0]
	stdin = stdin.split(boundary)
	content = b""
	msg = "Invalid submission"

	for block in stdin:
		if b"filename=" in block:
			split = block.split(b"\r\n")
			content = b""
			filename = ""
			content = split[-2]
			for line in split:
				if b"filename=" in line:
					filename = line.split(b'filename="', 1)[1].split(b'"')[0]
			if content and filename:
				if not os.path.isdir(b"./www/cgi/uploads/"):
					msg = "Upload directory doesn't exists"
				elif (os.path.isfile(b"./www/cgi/uploads/" + filename)):
					msg = "File already exists"
				else:
					with open(b"./www/cgi/uploads/" + filename, 'wb') as file:
						file.write(content)
					msg = "File uploaded"
					os.chmod("./www/cgi/uploads/" + str(filename.decode("ascii")), 0o777)

	html = """
	<!DOCTYPE html>
	<html lang="en" >

	<head>
	<meta charset="UTF-8">
	<title>Webserv | Python CGI</title>
	<link rel="stylesheet" type="text/css" href="../style.css"/>
	<link rel="icon" href="../images/favicon.ico" type="image/x-icon" />
	</head>

	<h1>""" + msg + """</h1>
	<div class="center">
	<a href="/">
		<button class="home-button">Home</button>
	</a>
	</div>
	<footer>
		<div class="wave"></div>
		<div class="wave"></div>
		<div class="wave"></div>
		<h2><a href="https://github.com/ktroude" target="_blank">ktroude</a>
			| <a href="https://github.com/YeetusTitus" target="_blank">jforner</a>
			| <a href="https://github.com/julien-ctx" target="_blank">jcauchet</a>
		</h2>
	</footer>
	</body>

	</html>
	"""
	print(html)
