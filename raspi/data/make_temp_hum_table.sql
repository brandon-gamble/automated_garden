CREATE TABLE `dual_zone_temp_hum` (
    `id` int(10) NOT NULL AUTO_INCREMENT PRIMARY KEY,
    `timestamp` varchar(50) NOT NULL,
    `z1_temp_f` float NOT NULL,
    `z1_hum` float NOT NULL,
    `z2_temp_f` float NOT NULL,
    `z2_hum` float NOT NULL
);
