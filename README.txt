AutoSurvCV Version 1.2 07/05/2017

KEY DIRECTORY PATHS:

C:/AutoSurvCV/record_log/
C:/AutoSurvCV/videos/
C:/AutoSurvCV/training/
C:/AutoSurvCV/opencv/

DEFAULT PATHS:
Training folder: C:/AutoSurvCV/training/PedCut2013/data/completeData/left_groundtruth/*.*

TESTING FILES:
C:/AutoSurvCV/test_bad.png
C:/AutoSurvCV/test_good.png

HOW TO USE AUTOSURVCV
----------------------------------------

1.	Unzip and install the AutoSurvCV directory to the C: drive.

2.	Ensure the folders are in the key file paths.

3.	Open command line, and navigate to C:\AutoSurvCV\x64\Release\

4.	Run 'AutoSurvCV.exe' and input the full path names for 
	the training folder and video file. If using a custom training
	folder, use '/' instead of '\', end training path with '*.*'


HOW TO RUN UNIT TESTS
----------------------------------------

1. 	Open AutoSurvCV in Visual Studio, if UnitTests fails to load,
	add the project to the AutoSurvCV project.

2.	Test > Test Settings > Default Processor Architecture > x64

3.	Run 'All Tests'