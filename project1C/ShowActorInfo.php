<html>
<body>
<a href = "http://192.168.56.20/~cs143/project1C/"> home </a><br>
<h2>Show Actor Information</h2>
	<?php 
	if($_GET["id"]){
		$id = $_GET["id"];
		//echo "$id<br>";
		$db_link = mysql_connect("localhost","cs143","");
		mysql_select_db('TEST',$db_link);
		if(!db_link){
			echo "Cannot connect to database!<br>";
			die('Could not connect: '.mysql_error());
		}
		$query = "select * from Actor where id=$id";
		$result = mysql_query($query, $db_link);
		if (!$result){
		die('Could not query the database'.mysql_error());
		}
		else{
			$row = mysql_fetch_row($result);
			echo "Name: $row[2] $row[1] <br>";
			echo "Sex: $row[3] <br>";
			echo "Date of Birth: $row[4] <br>";
			if($row[5])
				echo "Date of Death: $row[5] <br>";
			else
				echo "Date of Death: Still Alive<br>";
			echo "--Acted in--<br>";
			$query = "select Movie.id, Movie.title, Movie.year, MovieActor.role from MovieActor inner join Movie on MovieActor.mid=Movie.id where MovieActor.aid=$id order by Movie.title";
			//$query = "SELECT Movie.id, Movie.title from MovieActor INNER JOIN Movie ON MovieActor.mid=Movie.id WHERE MovieActor.aid=".$id." ORDER BY title;";
			//echo "$query<br>";
			$result = mysql_query($query, $db_link);
			if (!$result){
				die('Could not query the database'.mysql_error());
			}
			else{
				$row = mysql_fetch_row($result);
				if(!$row){
					echo "No record!<br>";
				}
				else{
					echo "<a href='./ShowMovieInfo.php?id=$row[0]'>$row[1]($row[2])</a> Acts as $row[3] <br>";
				}
				while($row=mysql_fetch_row($result)){
					echo "<a href='./ShowMovieInfo.php?id=$row[0]'>$row[1]($row[2])</a> Acts as $row[3] <br>";
				}
			}
		}
	}
	?>
</body>
</html>