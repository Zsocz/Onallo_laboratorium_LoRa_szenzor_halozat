<?php

	include("connect.php"); 	
	
	$link=Connection();

    $result=mysql_query("SELECT * FROM `current_sensor_data` ORDER BY `timeStamp` DESC",$link);
    if ($result !== false) {
        $data = mysql_fetch_array($result);
    }
?>


<!DOCTYPE html>
<html>

<head>
    <meta charset="utf-8">
    <title>LoRa mesh data</title>
    <link href="styles/styles.css" rel="stylesheet" />
    
</head>

<body class="dark">
    <div class="layout">
        <div class="header-text">LoRa sensor mesh data</div>
        <a class="archive_image"  href="archive.php"></a>
        <div class="content helth">

            <div class="row">
                <div class="legend-row">
                    <div class="legend">Air tempeature, °C</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="airTemp" class="label-value"></label>
                    <div class="data"> <?php printf("%s°C",$data["air_temp"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">Humidity, %</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="humidity" class="label-value"></label>
                    <div class="data"> <?php printf("%s%%",$data["humidity"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">Soil moisture, %</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="soilMoisture" class="label-value"></label>
                    <div class="data"> <?php printf("%s%%",$data["soil_moisture"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend"> Illuminance, lux</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="illuminance" class="label-value"></label>
                    <div class="data"> <?php printf("%slux",$data["lux"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">Lake water temprerature, °C</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="waterTemperature" class="label-value"></label>
                    <div class="data"> <?php printf("%s°C",$data["water_temp"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">Water level, High / Normal / Low</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="waterLevel" class="label-value"></label>
                    <div class="data"> <?php printf("%s",$data["water_level"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">UVA</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="UVA" class="label-value"></label>
                    <div class="data"> <?php printf("%s",$data["UVA"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">UVB</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="UVB" class="label-value"></label>
                    <div class="data"> <?php printf("%s",$data["UVB"]); ?></div>
                </div>
            </div>

            <div class="row">
                <div class="legend-row">
                    <div class="legend">UV index</div>
                    <div class="hr"></div>
                </div>
                <div class="clear"></div>
                <div class="col1">
                    <label id="UV_index" class="label-value"></label>
                    <div class="data"> <?php printf("%s",$data["UV_index"]); ?></div>
                </div>
            </div>

        </div>
    </div>

</body>

</html>