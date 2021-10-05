<?php
$cookie_name = "user";
$cookie_value = "John Doe";
setcookie($cookie_name, $cookie_value, time() + (86400 * 30), "/"); // 86400 = 1 day
?>

<html>

<body>
    <?php
    if (!isset($_COOKIE[$cookie_name])) {
        echo "<h3>Cookie named '" . $cookie_name . "' is not set!</h3>";
    } else {
        echo "<h3>Cookie '" . $cookie_name . "' is set!</h3><br>";
        echo "<h4>Value is: " . $_COOKIE[$cookie_name] . "</h4>";
    }
    ?>
</body>

</html>