<html>
<a href = "http://192.168.56.20/~cs143/project1C/"> home </a><br>
<h2>Add Person</h2>
<body>
	<form action = <?php echo $_SERVER['PHP_SELF']?> method="GET">
		Profession:
			<input type="radio" name="profession" value="actor">Actor
			<input type="radio" name="profession" value="director">Director
		<br>
		First Name: <input type="text" name="firstname"><br>
		Last Name:&nbsp<input type="text" name="lastname"><br>
		Sex:
			<input type="radio" name="sex" value="male">Male
			<input type="radio" name="sex" value="female">Female
		<br>
		Date of Birth:
		<?php include("DateofBirth.php"); ?>
		<br>
		Date of Death:
		<?php include("DateofDeath.php"); ?>
		<br>
		<input type="submit" name="submit" value="Submit">
	</form>

	<?php
	if($_GET["submit"]){
		$valid = true;
		if (empty($_GET["profession"])){
			echo "Profession is required<br>";
			$valid = false;
		}
		else if($_GET["profession"]=="actor"){
			$table="Actor";
		}
		else{
			$table="Director";
		}

		if (empty($_GET["firstname"])){
			echo "First name is required<br>";
			$valid = false;
		}
		else{
			$firstname = $_GET["firstname"];
			$firstname = str_replace("'","''",$firstname);
			$firstname = "'".$firstname."'";
		}

		if (empty($_GET["lastname"])){
			echo "Last name is required<br>";
			$valid = false;
		}
		else{
			$lastname = $_GET["lastname"];
			$lastname = str_replace("'","''",$lastname);
			$lastname = "'".$lastname."'";
		}
		if (empty($_GET["sex"])){
			echo "Sex is required<br>";
			$valid = false;
		}
		else if ($_GET["sex"]=="male") {
			$sex = "'"."Male"."'";
		}
		else{
			$sex = "'"."Female"."'";
		}

		if ($_GET["dob_month"]=="-" || $_GET["dob_day"]=="-" || $_GET["dob_year"]=="-"){
			echo "Date of birth is required<br>";
			$valid = false;
		}
		else{
			$dobM=$_GET["dob_month"];
			$dobD=$_GET["dob_day"];
			$dobY=$_GET["dob_year"];
			$dob = "'"."$dobM-$dobD-$dobY"."'";
		}

		if ($_GET["dod_month"]=="-" && $_GET["dod_day"]=="-" && $_GET["dod_year"]=="-"){
			$alive = true;
		}
		else if ($_GET["dod_month"]=="-" || $_GET["dod_day"]=="-" || $_GET["dod_year"]=="-"){
			echo "Date of death is invalid<br>";
			$valid = false;
		}
		else{
			$dodM=$_GET["dod_month"];
			$dodD=$_GET["dod_day"];
			$dodY=$_GET["dod_year"];
			$dod = "'"."$dodM-$dodD-$dodY"."'";
		}
		if(!$valid){
			exit(-1);
		}

		$db_link = mysql_connect("localhost","cs143","");
		mysql_select_db('TEST',$db_link);
		if(!db_link){
			echo "Cannot connect to database!<br>";
			die('Could not connect: '.mysql_error());
		}
		$query1 = "select max(id) from Actor";
		$result1 = mysql_query($query1, $db_link);
		if (!$result1){
		die('Could not query the database'.mysql_error());
		}
		else{
			$row1 = mysql_fetch_row($result1);
			$maxid1 = $row1[0];
		}
		$query2 = "select max(id) from Director";
		$result2 = mysql_query($query2, $db_link);
		if (!$result2){
		echo "Error when querying the database<br>";
		die('Could not query the database'.mysql_error());
		}
		else{
			$row2 = mysql_fetch_row($result2);
			$maxid2 = $row2[0];
		}
		if($maxid1>$maxid2){
			$addid = $maxid1+1;
		}
		else{
			$addid = $maxid2+1;
		}

		if($alive){
			$query = "insert into $table (id, last, first, sex, dob) values ($addid, $lastname, $firstname, $sex, $dob)";
			echo "$query<br>";
		}
		else{
			$query = "insert into $table (id, last, first, sex, dob, dod) values ($addid, $lastname, $firstname, $sex, $dob, $dod)";
			echo "$query<br>";
		}
		$result = mysql_query($query, $db_link);
		if(!result){
			die('Could not query the database'.mysql_error());
		}
		echo "Added Successfully!<br>";
		mysql_close($db_link);
	}
	?>
</body>
</html>