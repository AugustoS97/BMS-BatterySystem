(G-CODE GENERATED BY FLATCAM v8.994 - www.flatcam.org - Version Date: 2020/11/7)

(Name: EPSolar-CAN.drl_cnc)
(Type: G-code from Geometry)
(Units: MM)

(Created on Friday, 06 August 2021 at 00:59)

(This preprocessor is the default preprocessor used by FlatCAM.)
(It is made to work with MACH3 compatible motion controllers.)


(TOOLS DIAMETER: )
(Tool: 1 -> Dia: 0.76)
(Tool: 2 -> Dia: 0.8)
(Tool: 3 -> Dia: 1.0)
(Tool: 4 -> Dia: 1.52)
(Tool: 5 -> Dia: 3.2)
(Tool: 6 -> Dia: 3.2)
(Tool: 7 -> Dia: 3.2)
(Tool: 8 -> Dia: 3.2)

(FEEDRATE Z: )
(Tool: 1 -> Feedrate: 300)
(Tool: 2 -> Feedrate: 300)
(Tool: 3 -> Feedrate: 300)
(Tool: 4 -> Feedrate: 300)
(Tool: 5 -> Feedrate: 300)
(Tool: 6 -> Feedrate: 300)
(Tool: 7 -> Feedrate: 300)
(Tool: 8 -> Feedrate: 300)

(FEEDRATE RAPIDS: )
(Tool: 1 -> Feedrate Rapids: 1500)
(Tool: 2 -> Feedrate Rapids: 1500)
(Tool: 3 -> Feedrate Rapids: 1500)
(Tool: 4 -> Feedrate Rapids: 1500)
(Tool: 5 -> Feedrate Rapids: 1500)
(Tool: 6 -> Feedrate Rapids: 1500)
(Tool: 7 -> Feedrate Rapids: 1500)
(Tool: 8 -> Feedrate Rapids: 1500)

(Z_CUT: )
(Tool: 1 -> Z_Cut: -2.1)
(Tool: 2 -> Z_Cut: -2.1)
(Tool: 3 -> Z_Cut: -1.7)
(Tool: 4 -> Z_Cut: -1.7)
(Tool: 5 -> Z_Cut: -1.7)
(Tool: 6 -> Z_Cut: -1.7)
(Tool: 7 -> Z_Cut: -1.7)
(Tool: 8 -> Z_Cut: -1.7)

(Tools Offset: )
(Tool: 1 -> Offset Z: 0.0)
(Tool: 2 -> Offset Z: 0.0)

(Z_MOVE: )
(Tool: 1 -> Z_Move: 2.0)
(Tool: 2 -> Z_Move: 2.0)
(Tool: 3 -> Z_Move: 2)
(Tool: 4 -> Z_Move: 2)
(Tool: 5 -> Z_Move: 2)
(Tool: 6 -> Z_Move: 2)
(Tool: 7 -> Z_Move: 2)
(Tool: 8 -> Z_Move: 2)

(Z Toolchange: 15 mm)
(X,Y Toolchange: 0.0000, 0.0000 mm)
(Z Start: None mm)
(Z End: 0.5 mm)
(X,Y End: None mm)
(Steps per circle: 64)
(Preprocessor Excellon: default)

(X range:    2.2100 ...   77.8000  mm)
(Y range:    2.7750 ...   53.4160  mm)

(Spindle Speed: 0 RPM)
G21
G90
G94

G01 F300.00

M5
G00 Z15.0000
T1
G00 X0.0000 Y0.0000                
M6
(MSG, Change to Tool Dia = 0.7600 ||| Total drills for tool T1 = 57)
M0
G00 Z15.0000

G01 F300.00
M03
G00 X12.3190 Y13.7160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X14.8590 Y13.7160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X17.3990 Y13.7160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X19.9390 Y13.7160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X21.2090 Y16.2560
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X32.8930 Y19.3040
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X38.1000 Y9.9060
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X42.9800 Y9.9060
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X40.6400 Y3.1750
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X47.6250 Y6.7310
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X52.7050 Y11.3160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X48.8610 Y14.5680
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X44.4500 Y21.0820
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X50.2300 Y17.2250
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X52.7050 Y20.0660
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X51.9610 Y23.7000
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X52.1490 Y25.6190
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X53.8620 Y23.3870
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X57.1570 Y23.1660
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X56.7940 Y20.2260
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X58.7610 Y19.3170
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X61.9760 Y14.5780
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X67.6050 Y11.3160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X67.6050 Y20.0660
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X74.8030 Y36.3220
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X68.3150 Y46.1450
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X59.6900 Y39.6650
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X56.1480 Y39.3160
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X56.5460 Y36.5530
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X54.1940 Y37.4290
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X49.8420 Y40.7640
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X48.5400 Y37.9500
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X49.3640 Y34.6490
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X48.6500 Y32.0480
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X45.5770 Y36.2460
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X42.4830 Y36.7550
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X42.9800 Y48.0060
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X38.1000 Y48.0060
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X34.2900 Y42.7990
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X31.4960 Y44.0690
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X27.2750 Y38.8620
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X31.3690 Y37.2110
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X28.1940 Y31.1150
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X22.6060 Y26.5430
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X19.5580 Y27.4320
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X19.5580 Y29.9720
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X19.5580 Y32.5120
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X19.5580 Y35.0520
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X15.4940 Y34.9250
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X11.9380 Y35.0520
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X11.9380 Y32.5120
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X11.9380 Y29.9720
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X11.9380 Y27.4320
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X17.1450 Y19.9390
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X18.6690 Y16.2560
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X16.1290 Y16.2560
G01 Z-2.1000
G01 Z0
G00 Z2.0000
G00 X13.5890 Y16.2560
G01 Z-2.1000
G01 Z0
G00 Z2.0000
M05
G00 Z0.50


