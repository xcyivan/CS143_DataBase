<html>
<body>
<a href = "http://192.168.56.20/~cs143/project1C/"> home </a><br>
<h2>Search for Movie/Actor/Director</h2>
	<form action = <?php echo $_SERVER['PHP_SELF']?> method="GET">
		Search: <input type="search" name="search"><br>
		<input type="submit" name="submit" value="search">
	</form>

	<?php
	if($_GET["submit"]){
		if(empty($_GET["search"])){
			echo "please type anything you want to search in the search box<br>";
			exit(-1);
		}
		$db_link = mysql_connect("localhost","cs143","");
		mysql_select_db('TEST',$db_link);
		if(!db_link){
			echo "Cannot connect to database!<br>";
			die('Could not connect: '.mysql_error());
		}
		
		//search for actor
		$search = str_replace("'", "''", $_GET["search"]);
		$token = " ";
		$search_item = strtok($search, $token);
		$query_item = "";
		$query = "select first, last, dob, id from Actor where";
		if($search_item !==false){
			$query_item .= " (last like '%$search_item%' or first like '%$search_item%') ";
			$search_item = strtok($token);
			while($search_item !== false){
				$query_item .= "and (last like '%$search_item%' or first like '%$search_item%') ";
				$search_item = strtok($token);
			}
		}
		$query = $query.$query_item."order by first";
		//echo "$query<br>";
		$result = mysql_query($query, $db_link);
		if(!result){
			die('Could not query the database'.mysql_error());
		}
		echo "<b>Results for matched actors: </b><br>";
		while($row=mysql_fetch_row($result)){
			echo "<a href='./ShowActorInfo.php?id=$row[3]'>Actor: $row[0] $row[1] ($row[2])</a><br>";
		}
		echo "<br>";

		//search for director
		$search = str_replace("'", "''", $_GET["search"]);
		$token = " ";
		$search_item = strtok($search, $token);
		$query_item = "";
		$query = "select first, last, dob, id from Director where";
		if($search_item !==false){
			$query_item .= " (last like '%$search_item%' or first like '%$search_item%') ";
			$search_item = strtok($token);
			while($search_item !== false){
				$query_item .= "and (last like '%$search_item%' or first like '%$search_item%') ";
				$search_item = strtok($token);
			}
		}
		$query = $query.$query_item."order by first";
		//echo "$query<br>";
		$result = mysql_query($query, $db_link);
		if(!result){
			die('Could not query the database'.mysql_error());
		}
		echo "<b>Results for matched directors: </b><br>";
		while($row=mysql_fetch_row($result)){
			echo "<a href='./ShowDirectorInfo.php?id=$row[3]'>Director: $row[0] $row[1] ($row[2])</a><br>";
		}
		echo "<br>";

		//search for movie
		$search = str_replace("'", "''", $_GET["search"]);
		$token = " ";
		$search_item = strtok($search, $token);
		$query_item = "";
		$query = "select title, year, id from Movie where ";
		if($search_item !==false){
			$query_item .= " title like '%$search_item%' ";
			$search_item = strtok($token);
			while($search_item !== false){
				$query_item .= " and title like '%$search_item%' ";
				$search_item = strtok($token);
			}
		}
		$query = $query.$query_item."order by title";
		//echo "$query<br>";
		$result = mysql_query($query, $db_link);
		if(!result){
			die('Could not query the database'.mysql_error());
		}
		echo "<b>Results for matched movies: </b><br>";
		while($row=mysql_fetch_row($result)){
			echo "<a href='./ShowMovieInfo.php?id=$row[2]'>Movie: $row[0]($row[1])</a><br>";
		}
		echo "<br>";

		mysql_close($db_link);
	}
	?>


</body>
</html>