<html>
<a href = "http://192.168.56.20/~cs143/project1C/"> home </a><br>
<h2>Add Relation</h2>
<body>
	<form action = <?php echo $_SERVER['PHP_SELF']?> method="GET">
		Movie:
		<select name="mid">
		<option value="-">-</option><br>
		<?php 
			$db_link = mysql_connect("localhost","cs143","");
			mysql_select_db('TEST',$db_link);
			if(!db_link){
				echo "Cannot connect to database!<br>";
				die('Could not connect: '.mysql_error());
			}
			$query = "select id, title, year from Movie order by title";
			$result = mysql_query($query, $db_link);
			if (!$result){
				die('Could not query the database'.mysql_error());
			}
			while($row = mysql_fetch_row($result)){
				echo "<option value=$row[0]>"."$row[1]($row[2])"."</option>";
			}
			mysql_close($db_link);
		?>
		</select>
		<br>
		Actor:
		<select name="aid">
		<option value="-">-</option><br>
		<?php 
			$db_link = mysql_connect("localhost","cs143","");
			mysql_select_db('TEST',$db_link);
			if(!db_link){
				echo "Cannot connect to database!<br>";
				die('Could not connect: '.mysql_error());
			}
			$query = "select id, first, last, dob from Actor order by first";
			$result = mysql_query($query, $db_link);
			if (!$result){
				die('Could not query the database'.mysql_error());
			}
			while($row = mysql_fetch_row($result)){
				echo "<option value=$row[0]>"."$row[1] $row[2] ($row[3])"."</option>";
			}
			mysql_close($db_link);
		?>
		</select>
		<br>
		Role: <input type="text" name="role">(Leave blank if not known)<br>
		<input type="submit" name="submit" value="Submit">
	</form>

	<?php
	if($_GET["submit"]){
		$valid=true;
		if($_GET["mid"]=="-"){
			echo "Error! Must select one move!<br>";
			$valid=false;
		}
		else{
			$mid=$_GET["mid"];
		}

		if($_GET["aid"]=="-"){
			echo "Error! Must select one actor!<br>";
			$valid=false;
		}
		else{
			$aid=$_GET["aid"];
		}

		if(empty($_GET["role"])){
			echo "Error! Must enter the role!<br>";
			$valid=false;
		}
		else{
			$role=$_GET["role"];
			$role=str_replace("'", "''", $role);
			$role="'".$role."'";
		}
		if(!$valid){
			exit(-1);
		}
		//echo "$mid, $aid, $role<br>";
		$db_link = mysql_connect("localhost","cs143","");
		mysql_select_db('TEST',$db_link);
		if(!db_link){
			echo "Cannot connect to database!<br>";
			die('Could not connect: '.mysql_error());
		}
		$query="insert into MovieActor (mid, aid, role) values ($mid, $aid, $role)";
		echo "$query<br>";
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









