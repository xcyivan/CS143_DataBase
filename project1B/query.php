<html>
<title> Query MySQL</title>
<h2> Let's query MySQL:</h2>

<form id="query" method="post" action="<?php echo htmlspecialchars($_SERVER['PHP_SELF']);?>">
<TEXTAREA NAME="area" ROWS=10 COLS=60 value="Enter your query here" align="left">
</TEXTAREA><br>
<input type="submit" value="submit">
</form>
<br>
	<?php 
		if($_SERVER["REQUEST_METHOD"]=="POST"){
			echo "<h2>Results from MySQL:</h2>";
			//establish connection
			$db_link = mysql_connect("localhost","cs143","");
			mysql_select_db("TEST",$db_link);
			if(!$db_link){
				die('Could not connect: '.mysql_error());
			}

			$query = $_POST["area"];
			$sanitized_query=mysql_real_escape_string($query,$db_link);
			$rs = mysql_query($sanitized_query, $db_link);
			if(!rs){
				die('Invalid query: '.mysql_error());
			}
			echo "<table border='1'>";
			while($row = mysql_fetch_row($rs)){
				echo "<tr>";
				for($i=0; $i<count($row);$i++){
					echo "<td>$row[$i]</td>";
				}
				echo "</tr>";
			}
			echo "</table>";
			mysql_close($db_link);
		}
	?>
</html>














