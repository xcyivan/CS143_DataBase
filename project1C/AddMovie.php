<html>
<a href = "http://192.168.56.20/~cs143/project1C/"> back </a><br>
<h2>Add Movie</h2>
<body>
	<form action = <?php echo $_SERVER['PHP_SELF']?> method="GET">
		Title: <input type="text" name="title"><br>
		Company:<input type="text" name="company"><br>
		Year:
		<?php include("MovieYear.php"); ?>
		<br>
		MAAP Rating:
		<select name = "rating">
			<option value="-">-</option>
			<option value="PG-13">PG-13</option>
			<option value="R">R</option>
			<option value="PG">PG</option>
			<option value="NG-17">NG-17</option>
			<option value="surrendere">surrendere</option>
			<option value="G">G</option>
		</select><br>
		Genre:<br>
			<input type="checkbox" name="genre[]" value="Drama">Drama
			<input type="checkbox" name="genre[]" value="Comedy">Comedy
			<input type="checkbox" name="genre[]" value="Romance">Romance
			<input type="checkbox" name="genre[]" value="Crime">Crime
			<input type="checkbox" name="genre[]" value="Horror">Horror<br>
			<input type="checkbox" name="genre[]" value="Mystery">Mystery
			<input type="checkbox" name="genre[]" value="Thriller">Thriller
			<input type="checkbox" name="genre[]" value="Action">Action
			<input type="checkbox" name="genre[]" value="Adventure">Adventure
			<input type="checkbox" name="genre[]" value="Fantasy">Fantasy<br>
			<input type="checkbox" name="genre[]" value="Documentary">Documentary
			<input type="checkbox" name="genre[]" value="Family">Family
			<input type="checkbox" name="genre[]" value="Sci-Fi">Sci-Fi
			<input type="checkbox" name="genre[]" value="Animation">Animation
			<input type="checkbox" name="genre[]" value="Musical">Musical<br>
			<input type="checkbox" name="genre[]" value="War">War
			<input type="checkbox" name="genre[]" value="Western">Western
			<input type="checkbox" name="genre[]" value="Adult">Adult
			<input type="checkbox" name="genre[]" value="Short">Short
		<br>
		<input type="submit" name="submit" value="Submit">
	</form>

	<?php
	if($_GET["submit"]){
		$valid = true;
		if(empty($_GET["genre"])){
			echo "Error! Must select at least one genre!<br>";
			$valid = false;
		}

		if(empty($_GET["title"])){
			echo "Error! Must enter a title for the movie!<br>";
			$valid = false;
		}
		else{
			$title = "'".$_GET["title"]."'";
		}

		if(empty($_GET["company"])){
			echo "Error! Must enter a company for the movie!<br>";
			$valid = false;
		}
		else{
			$company = "'".$_GET["company"]."'";
		}

		if($_GET["movie_year"]=="-"){
			echo "Error! Must select a year for the movie!<br>";
			$valid = false;
		}
		else{
			$Myear = $_GET["movie_year"];
		}

		if(empty($_GET["rating"])){
			echo "Error! Must select a rating for the movie!<br>";
			$valid = false;
		}
		else{
			$rating = "'".$_GET["rating"]."'";
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

		//add to Table Movie
		$query0 = "select max(id) from Movie";
		$result0 = mysql_query($query0, $db_link);
		if (!$result0){
		die('Could not query the database'.mysql_error());
		}
		else{
			$row0 = mysql_fetch_row($result0);
			$addid0 = $row0[0]+1;
		}
		$query = "insert into Movie (id, title, year, rating, company) values ($addid0, $title, $Myear, $rating, $company)";
		//echo "$query<br>";
		$result = mysql_query($query, $db_link);
		if(!result){
			die('Could not query the database'.mysql_error());
		}
		else{
			echo "Added Successfully to Movie Table!<br>";
		}

		//add to Table MovieGenre
		foreach($_GET['genre'] as $g) {
			$g = "'".$g."'";
			$query = "insert into MovieGenre(mid, genre) values ($addid0, $g)";
			$addid1 += 1;
			//echo "$query<br>";
			$result = mysql_query($query, $db_link);
			if(!result){
				die('Could not query the database'.mysql_error());
			}
		}
		echo "Added Successfully to MovieGenre Table!<br>";
	}
	?>
</body>
</html>