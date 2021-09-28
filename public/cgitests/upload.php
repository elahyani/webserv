<?php

print_r($_FILES);
print_r($_POST);
// Check if image file is a actual image or fake image
if (isset($_POST)) {
	echo "OK";
	move_uploaded_file($_FILES["t"]["tmp_name"], "../toUpload/" . $_FILES["t"]["name"]);
}
?>