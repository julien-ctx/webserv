#!/usr/local/bin/python3

if __name__ == "__main__":
	html = """
	<!DOCTYPE html>
	<html lang="en" >

	<head>
	<meta charset="UTF-8">
	<title>Webserv | Python CGI</title>
	<link rel="stylesheet" type="text/css" href="../style.css"/>
	<link rel="icon" href="../images/favicon.ico" type="image/x-icon" />
	</head>

	<h1>42</h1>
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
