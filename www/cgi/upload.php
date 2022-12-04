#!/opt/homebrew/bin/php

<?php
if ($_SERVER["REQUEST_METHOD"] == "POST")
{
    if (isset($_FILES["file"]))
      echo "yes";
    else
      echo "couldn't retrieve file content";
}
else
{
    echo "File upload failed\n";
}
?>
