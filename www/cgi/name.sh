#!/bin/bash

echo '<!DOCTYPE html>
<html lang="en" >

<head>
<meta charset="UTF-8">
<title>Webserv | Python CGI</title>
<link rel="stylesheet" type="text/css" href="../style.css"/>
<link rel="icon" href="../images/favicon.ico" type="image/x-icon" />
</head>'

echo "<h1>You are $USER!</h1>"

echo '<div class="center">
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

</html>'
