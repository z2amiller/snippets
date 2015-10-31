EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_01X03 P2
U 1 1 5634F504
P 1100 3100
F 0 "P2" H 1100 3300 50  0000 C CNN
F 1 "CONN_01X03" V 1200 3100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 1100 3100 60  0001 C CNN
F 3 "" H 1100 3100 60  0000 C CNN
	1    1100 3100
	-1   0    0    1   
$EndComp
$Comp
L CONN_01X03 P1
U 1 1 5634F578
P 1100 2600
F 0 "P1" H 1100 2800 50  0000 C CNN
F 1 "CONN_01X03" V 1200 2600 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 1100 2600 60  0001 C CNN
F 3 "" H 1100 2600 60  0000 C CNN
	1    1100 2600
	-1   0    0    1   
$EndComp
$Comp
L MMBT3904 Q1
U 1 1 5634F5BA
P 2700 2100
F 0 "Q1" H 2900 2175 50  0000 L CNN
F 1 "MMBT3904" H 2900 2100 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 2900 2025 50  0000 L CIN
F 3 "" H 2700 2100 50  0000 L CNN
	1    2700 2100
	1    0    0    -1  
$EndComp
$Comp
L MMBT3904 Q2
U 1 1 5634F699
P 2700 2650
F 0 "Q2" H 2900 2725 50  0000 L CNN
F 1 "MMBT3904" H 2900 2650 50  0000 L CNN
F 2 "TO_SOT_Packages_SMD:SOT-23" H 2900 2575 50  0000 L CIN
F 3 "" H 2700 2650 50  0000 L CNN
	1    2700 2650
	1    0    0    -1  
$EndComp
$Comp
L LD3985G47R U1
U 1 1 5634F76A
P 2850 3400
F 0 "U1" H 2600 3600 40  0000 C CNN
F 1 "LD3985G47R" H 3050 3600 40  0000 C CNN
F 2 "TO_SOT_Packages_SMD:SOT-23-5" H 2850 3500 35  0000 C CIN
F 3 "" H 2850 3400 60  0000 C CNN
	1    2850 3400
	1    0    0    -1  
$EndComp
Text GLabel 1300 2700 2    39   Input ~ 0
LEFT_1
Text GLabel 1300 2600 2    39   Input ~ 0
LEFT_2
Text GLabel 1300 2500 2    39   Input ~ 0
LEFT_3
Text GLabel 1300 3000 2    39   Input ~ 0
RIGHT_3
Text GLabel 1300 3100 2    39   Input ~ 0
RIGHT_2
Text GLabel 1300 3200 2    39   Input ~ 0
RIGHT_1
Text GLabel 3050 2450 2    39   Input ~ 0
LEFT_1
Text GLabel 3050 2850 2    39   Input ~ 0
RIGHT_1
Text GLabel 2500 2650 0    39   Input ~ 0
RIGHT_2
Wire Wire Line
	2800 2450 3050 2450
Wire Wire Line
	2800 2850 3050 2850
Text GLabel 3050 1900 2    39   Input ~ 0
RIGHT_3
Text GLabel 2500 2100 0    39   Input ~ 0
LEFT_2
Text GLabel 3050 2300 2    39   Input ~ 0
LEFT_3
Wire Wire Line
	2800 1900 3050 1900
Wire Wire Line
	2800 2300 3050 2300
Text GLabel 2400 3500 0    39   Input ~ 0
LEFT_1
Text GLabel 2850 3700 3    39   Input ~ 0
LEFT_2
Text GLabel 2400 3350 0    39   Input ~ 0
LEFT_3
Text GLabel 3300 3500 2    39   Input ~ 0
RIGHT_1
Text GLabel 3300 3350 2    39   Input ~ 0
RIGHT_3
$EndSCHEMATC
