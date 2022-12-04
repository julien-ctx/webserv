#!/opt/homebrew/bin/php

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    echo var_dump($_FILES);
}
else
{
    echo "File upload failed\n";
}
?>
