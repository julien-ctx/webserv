#!/usr/local/bin/python3
import sys
import os

if __name__ == "__main__":
	stdin = sys.stdin.buffer.read()
	if not stdin:
		exit()
	boundary = stdin.split(b'\n')[0]
	stdin = stdin.split(boundary)
	content = b""
	msg = "Invalid submission"
	cgi_dir = stdin[-1].split(b"\r\n")[-1] + b"/"
	
	for block in stdin[1:]:
		if b"filename=" in block:
			split = block.split(b"\r\n")
			content = b""
			filename = ""
			content = b"\r\n".join(split[3:-1])
			for line in split:
				if b"filename=" in line:
					filename = line.split(b'filename="', 1)[1].split(b'"')[0]
			if filename:
				if not os.path.isdir(cgi_dir):
					os.makedirs(cgi_dir)	
				if (os.path.isfile(cgi_dir + filename)):
					msg = "File already exists"
				else:
					with open(cgi_dir + filename, 'wb') as file:
						file.write(content)
					msg = "File uploaded"
					os.chmod(cgi_dir + filename, 0o777)

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
