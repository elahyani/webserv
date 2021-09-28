<?php

if (isset($_POST))
    print_r($_POST);

?>
<form method="POST">
    <input type="text" name="username">
    <button>Submit</button>
</form>