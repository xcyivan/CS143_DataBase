<html>
<title>My Calculator</title>
<h1>Calculator</h1>
<form method="get" action = "<?php echo htmlspecialchars($_SERVER['PHP_SELF']); ?>">
<input type="text" name="input">
<input type="submit" value="Calculate"><br>
</form>
<?php
	function isvalid_input($input){
		$regexp="/^-? *\d+(.\d+)? *( *[\/+\-*] *-?\d+(.\d+)?)*$/";
		if(preg_match($regexp, $input))
			return true;
		else
			return false;
	}
	$input="";
	$report="Report";
	$output="";
	if($_SERVER["REQUEST_METHOD"]=="GET"){
		$input=trim($_GET["input"]);
	}
	if(isvalid_input($input)){
		if(preg_match("/\/ *0(.0+)*/", $input)){
			$output = "Division by zero error!";
		}
		else{
		eval('$output = '.$input.';');
		$output = $input."=".$output;
		}
	}
	else{
		$output="Invalid Expression!";
	}
	if(strlen($input)>0){
		echo "<h2>$report</h2><br>";
		echo "$output";
	}
?>
</html>