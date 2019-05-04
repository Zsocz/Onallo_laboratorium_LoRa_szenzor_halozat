<?php

include "connect.php";

$link = Connection();

$result = mysql_query("SELECT * FROM `sensor_data` ORDER BY `timeStamp` DESC", $link);
?>

<!DOCTYPE html>
<html>

<head>
    <meta charset="utf-8">
    <title>LoRa mesh data archive</title>
    <link href="styles/styles.css" rel="stylesheet" />

</head>

<body class="dark">
    <div class="archive_layout">
        <div class="header-text">Data archive</div>
        <a class="index_image"  href="index.php"></a>
    </div>

    <table >
		<tr>
			<th>&nbsp;Timestamp&nbsp;</th>
			<th>&nbsp;Temperature&nbsp;</th>
            <th>&nbsp;Humidity&nbsp;</th>
            <th>&nbsp;Soil moisture&nbsp;</th>
			<th>&nbsp;Luminance&nbsp;</th>
            <th>&nbsp;Water Temp&nbsp;</th>
            <th>&nbsp;Water level&nbsp;</th>
			<th>&nbsp;UVA&nbsp;</th>
            <th>&nbsp;UVB&nbsp;</th>
            <th>&nbsp;UV index&nbsp;</th>
		</tr>

      <?php
if ($result !== false) {
    while ($row = mysql_fetch_array($result)) {
        printf("<tr> <td>&nbsp;%s&nbsp;</td> <td>&nbsp;%s&nbsp;°C</td> <td>&nbsp;%s&nbsp;%%</td> <td>&nbsp;%s&nbsp;%%</td> <td>&nbsp;%s&nbsp;lux</td> <td>&nbsp;%s&nbsp;°C</td> <td>&nbsp;%s&nbsp;</td> <td>&nbsp;%s&nbsp;</td> <td>&nbsp;%s&nbsp;</td> <td>&nbsp;%s&nbsp;</td> </tr>",
            $row["timeStamp"], $row["air_temp"], $row["humidity"],
            $row["soil_moisture"], $row["lux"], $row["water_temp"],
            $row["water_level"], $row["UVA"], $row["UVB"], $row["UV_index"]);
    }
    mysql_free_result($result);
    mysql_close();
}
?>

   </table>



</body>

</html>