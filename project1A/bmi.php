<html>
<title> BMI Calculator </title>
<body>
	<h3 align="center">
		Body Mass Index Calculator<br>
	</h3>

	<form id="BMICal" method="post" action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
		<p align="center">
			<?php
			$Height=$Weight="";
			$BMI=0;
			if ($_SERVER["REQUEST_METHOD"] == "POST") {
				$Height = floatval($_POST['Height']);
				$Weight = floatval($_POST['Weight']);
				$BMI = round($Weight/($Height*$Height),2);
			}
			?>
			Height (m): <input type="text" name="Height" value="<?php echo $Height; ?>" maxlength="3" style='width:45px'> 
			Weight (Kg): <input type="text" name="Weight" value="<?php echo $Weight; ?>" maxlength="3" style='width:45px'><br><br>
			BMI: <input type="text" value="<?php echo $BMI; ?>" maxlength="5" style='width:45px' readonly><br>
			<input type="submit" value="Calculate BMI">
			<hr>
		</p>
	</form>
	<b>Report:<br></b>
	<?php
	if ($_SERVER["REQUEST_METHOD"] == "POST") {
		if($BMI<=18.5)
			$result="Underweight";
		elseif($BMI<=24.9)
			$result="Normal weight";
		elseif($BMI<=29.9)
			$result="Overweight";
		elseif($BMI<=39.9)
			$result="Obesity";
		else
			$result="Morbidly Obesity";
		echo "You are $result.<br>";
	}
	?>
</body>
</html>
