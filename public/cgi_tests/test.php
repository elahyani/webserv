<h1><?php
    if (isset($_POST))
        print_r($_POST)

    ?></h1>
<form method="POST">
    <input type="text" name="username">
    <button>Submit</button>
</form>