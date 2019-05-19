EESchema Schematic File Version 4
LIBS:Power Board-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "IRIS2: Power distribution board"
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Barrel_Jack_MountingPin J1
U 1 1 5C6FCBD8
P 2850 2500
F 0 "J1" H 2905 2817 50  0000 C CNN
F 1 "Barrel_Jack_MountingPin" H 2905 2726 50  0000 C CNN
F 2 "Connector_BarrelJack:Barrel Jack Vertical" H 2900 2460 50  0001 C CNN
F 3 "~" H 2900 2460 50  0001 C CNN
	1    2850 2500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J2
U 1 1 5C6FCC2E
P 3650 1800
F 0 "J2" V 3616 1612 50  0000 R CNN
F 1 "Conn_01x02" V 3525 1612 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B02B-EH-A_1x02_P2.50mm_Vertical" H 3650 1800 50  0001 C CNN
F 3 "~" H 3650 1800 50  0001 C CNN
	1    3650 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 5C6FCC92
P 4350 1800
F 0 "J3" V 4316 1612 50  0000 R CNN
F 1 "Conn_01x02" V 4225 1612 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B02B-EH-A_1x02_P2.50mm_Vertical" H 4350 1800 50  0001 C CNN
F 3 "~" H 4350 1800 50  0001 C CNN
	1    4350 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 5C6FCCB7
P 5050 1800
F 0 "J4" V 5016 1612 50  0000 R CNN
F 1 "Conn_01x02" V 4925 1612 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B02B-EH-A_1x02_P2.50mm_Vertical" H 5050 1800 50  0001 C CNN
F 3 "~" H 5050 1800 50  0001 C CNN
	1    5050 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J5
U 1 1 5C6FCCDB
P 5800 1800
F 0 "J5" V 5766 1612 50  0000 R CNN
F 1 "Conn_01x02" V 5675 1612 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B02B-EH-A_1x02_P2.50mm_Vertical" H 5800 1800 50  0001 C CNN
F 3 "~" H 5800 1800 50  0001 C CNN
	1    5800 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J6
U 1 1 5C6FCD05
P 6500 1800
F 0 "J6" V 6466 1612 50  0000 R CNN
F 1 "Conn_01x02" V 6375 1612 50  0000 R CNN
F 2 "Connector_JST:JST_VH_B2P-VH-B_1x02_P3.96mm_Vertical" H 6500 1800 50  0001 C CNN
F 3 "~" H 6500 1800 50  0001 C CNN
	1    6500 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J7
U 1 1 5C6FCD2D
P 7200 1800
F 0 "J7" V 7166 1612 50  0000 R CNN
F 1 "Conn_01x02" V 7075 1612 50  0000 R CNN
F 2 "Connector_JST:JST_VH_B2P-VH-B_1x02_P3.96mm_Vertical" H 7200 1800 50  0001 C CNN
F 3 "~" H 7200 1800 50  0001 C CNN
	1    7200 1800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3150 2400 3650 2400
Wire Wire Line
	7200 2400 7200 2000
Wire Wire Line
	3650 2000 3650 2400
Connection ~ 3650 2400
Wire Wire Line
	3650 2400 4350 2400
Wire Wire Line
	4350 2000 4350 2400
Connection ~ 4350 2400
Wire Wire Line
	4350 2400 5050 2400
Wire Wire Line
	5050 2000 5050 2400
Connection ~ 5050 2400
Wire Wire Line
	5050 2400 5800 2400
Wire Wire Line
	5800 2400 5800 2000
Connection ~ 5800 2400
Wire Wire Line
	5800 2400 6500 2400
Wire Wire Line
	6500 2000 6500 2400
Connection ~ 6500 2400
Wire Wire Line
	6500 2400 7200 2400
Wire Wire Line
	3150 2600 3300 2600
Wire Wire Line
	3750 2600 3750 2000
Wire Wire Line
	3750 2600 4450 2600
Wire Wire Line
	4450 2600 4450 2000
Connection ~ 3750 2600
Wire Wire Line
	4450 2600 5150 2600
Wire Wire Line
	5150 2600 5150 2000
Connection ~ 4450 2600
Wire Wire Line
	5150 2600 5900 2600
Wire Wire Line
	5900 2600 5900 2000
Connection ~ 5150 2600
Wire Wire Line
	5900 2600 6600 2600
Wire Wire Line
	6600 2600 6600 2000
Connection ~ 5900 2600
Wire Wire Line
	6600 2600 7300 2600
Wire Wire Line
	7300 2600 7300 2000
Connection ~ 6600 2600
Wire Wire Line
	2850 2800 2850 2900
Wire Wire Line
	2850 2900 3300 2900
Wire Wire Line
	3300 2900 3300 2600
Connection ~ 3300 2600
Wire Wire Line
	3300 2600 3750 2600
$Comp
L Connector_Generic:Conn_01x02 J8
U 1 1 5CC866AE
P 7950 1800
F 0 "J8" V 7916 1612 50  0000 R CNN
F 1 "Conn_01x02" V 7825 1612 50  0000 R CNN
F 2 "Connector_JST:JST_VH_B2P-VH-B_1x02_P3.96mm_Vertical" H 7950 1800 50  0001 C CNN
F 3 "~" H 7950 1800 50  0001 C CNN
	1    7950 1800
	0    -1   -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J9
U 1 1 5CC866D9
P 8700 1800
F 0 "J9" V 8666 1612 50  0000 R CNN
F 1 "Conn_01x02" V 8575 1612 50  0000 R CNN
F 2 "Connector_JST:JST_VH_B2P-VH-B_1x02_P3.96mm_Vertical" H 8700 1800 50  0001 C CNN
F 3 "~" H 8700 1800 50  0001 C CNN
	1    8700 1800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7200 2400 7950 2400
Wire Wire Line
	7950 2400 7950 2000
Connection ~ 7200 2400
Wire Wire Line
	7950 2400 8700 2400
Wire Wire Line
	8700 2400 8700 2000
Connection ~ 7950 2400
Wire Wire Line
	7300 2600 8050 2600
Wire Wire Line
	8050 2600 8050 2000
Connection ~ 7300 2600
Wire Wire Line
	8050 2600 8800 2600
Wire Wire Line
	8800 2600 8800 2000
Connection ~ 8050 2600
$EndSCHEMATC
