<html>
<body>
<a href = "http://192.168.56.20/~cs143/project1C/"> home </a><br>
<h2>Show Movie Information</h2>
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
		$query = "select * from Movie where id=$id";
		$result = mysql_query($query, $db_link);
		if (!$result){
		die('Could not query the database'.mysql_error());
		}

		$row = mysql_fetch_row($result);
			echo "Title: $row[1] <br>";
			echo "Year: $row[2] <br>";
			echo "MPAA Rating: $row[3] <br>";
			echo "Producer: $row[4] <br>";
			echo "Director: ";
				$query = "select Director.first, Director.last, Director.id from MovieDirector inner join Director on MovieDirector.did=Director.id where MovieDirector.mid=$id order by Director.first";
				$result = mysql_query($query, $db_link);
				if (!$result){
					die('Could not query the database'.mysql_error());
				}
				$row = mysql_fetch_row($result);
				echo "<a href='./ShowDirectorInfo.php?id=$row[2]'>$row[0] $row[1]</a><br>";
			echo "Genre: " ;
				$query = "select genre from MovieGenre where mid=$id";
				$result = mysql_query($query, $db_link);
				if (!$result){
					die('Could not query the database'.mysql_error());
				}
				while($row=mysql_fetch_row($result)){
					echo "$row[0] &nbsp ";
				}
			echo "<br>";
			echo "----Actors in this Movie----<br>";
				$query = "select Actor.first, Actor.last, MovieActor.role, Actor.id from Actor inner join MovieActor on MovieActor.aid=Actor.id where MovieActor.mid=$id order by Actor.first";
				$result = mysql_query($query, $db_link);
				if (!$result){
					die('Could not query the database'.mysql_error());
				}
				while($row=mysql_fetch_row($result)){
					echo "<a href='./ShowActorInfo.php?id=$row[3]'>$row[0] $row[1] </a>acts as $row[2]<br>";
				}


			echo "<br>";
	}
	?>
</body>
</html>