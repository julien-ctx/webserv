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
	msg = "File uploaded"

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
				if not os.path.isdir("./www/cgi/uploads/"):
					msg = "Upload directory doesn't exists"
				if (os.path.isfile("./www/cgi/uploads/" + filename)):
					msg = "File already exists!"
				with open("./www/cgi/uploads/" + filename, 'w') as file:
					file.write(content)
			else:
				msg = "Invalid submission"

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
