<?php



session_start();

// print_r($_POST);
if (isset($_POST['login'])) {
	if ($_POST['username'] == 'itree' && $_POST['password'] == '1111') {
		$_SESSION['user'] = [
			'username' => $_POST['username'],
			'password' => $_POST['password']
		];
	}
}

if (isset($_POST['logout'])) {
	unset($_SESSION['user']);
	session_destroy();
}

?>

<!DOCTYPE html>
<html lang="en">

<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" integrity="sha384-EVSTQN3/azprG1Anm3QDgpJLIm9Nao0Yz1ztcQTwFspd3yD65VohhpuuCOmLASjC" crossorigin="anonymous">
	<title>Document</title>
	<style>
		html {
			font-family: sans-serif;
			height: 100%;
		}

		body {
			height: 100vh;
			color: #FFB399;
		}

		.padd {
			margin-right: 70px;
		}
	</style>
</head>

<body class="text-center">
	<div class="position-absolute bottom-50 start-50 translate-middle padd">
		<?php if (!isset($_SESSION['user'])) {  ?>
			<div class="border border-danger p-3 rounded">
				<h2 class="">Authentication</h2>
				<form method="POST">
					<div class="mb-3">
						<label for="username" class="form-label" style="float: left;">Username</label>
						<input type="text" class="form-control padd" name="username">
					</div>
					<div class="mb-3">
						<label for="password" class="form-label" style="float: left;">Password</label>
						<input type="password" class="form-control padd" name="password">
					</div>
					<button type="submit" name="login" class="btn btn-danger w-100 rounded-pill mt-2 p-2">Submit</button>
				</form>
			<?php } else {  ?>
				<h1>Your are connected</h1>
				<form method="POST">
					<button type="submit" name="logout" class="btn btn-danger w-100 rounded-pill mt-2 p-2">Logout</button>
				</form>
			<?php } ?>
			</div>
	</div>
</body>

</html>