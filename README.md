# READ ME

## About the coursework:

The objective of this coursework was to complete 4 different tasks:
- Task 1: effectively use Git and Source Control with commits and tags
- Task 2: Create C code for a temperature sensor that uses discrete fourier transform to optimize sampling rate
- Task 3: Optimize a series of robots that deliver pizza in a virtual environment and improve Key Performance Indicators (KPIs) by factoring in multiple features of said classes of robots, such as their carrying weight, charge, and mobility and creating functions that optimized their decision making on what pizzas to collect and whether to charge.
- Task 4: Develop a python program that reads data from task 2 through a CSV file, and provide a written analysis regarding the plots, any noticeable trends, and an analysis of the variable power modes and their effectiveness.

## Structure of Commits
For this project, commit titles and structure was based on the Conventional Commit convention.

## File Locations:

### For Task 2:
The file with temperature sensor optimization code is labelled "temperature_optimization.ino" found in the src folder within the arduino folder. For this coursework, the Platform.io IDE was used to build and upload code onto the arduino, and it is heavily advised that for testing and analysis of my code, this IDE is used.

### For Task 3:
The file with optimizations as laid out by the task is titled robot_optimization, and is found in the robots folder. utilize the run command in the venv  python.exe -m robots.robot_optimization or if on mac (which is what i used): 
python -m robots.robot_optimization

In order to find the tabulated data, simply run the program file and it will output the table once the environment has been successfully run.

IMPORTANT: ensure the python environment is in a venv before running the file otherwise the required modules will not load properly

If it appears to buffer, do not click away or try to refresh, KEEP LETTING IT RUN. it takes time to generate the multiple environments for the table, so let it take its time and the table with tabitulated comparison data should be the output of the terminal.

Written analysis is in the robot_optimization_analysis.md file with a table and a short annotation on why said tabitulated results are the way they are.

### For Task 4:
Multiple files were created for the written analysis and program to read the CSV File.

- data_analyser.py is the python program written using pandas and matplotlib to generate 5 plots comparing time, temperature, magnitude, as well as other parameters required of the task
- task 4 full analysis.md is my written analysis of each plot as well as questions that were suggested for analysis, please use this when viewing my analysis as required by task 4
- temp_data.csv is the csv file used by data_analyser.py to be graphed using pandas and matplotlib
- temp_data_maker.ino is a modified version of temperature_optimization.ino, with the send_data_to_pc function modified to be easily transferrable to the csv file.
- any pngs are pngs of each individual plot.

## (NOTICE) AI as a tool:

Claude was used throughout this coursework to help explain concepts and improve the quality of my code through back-and-forth dialogue, also known as Socratic prompting. All code is self written with refinements recommended from Claude. For this coursework, I encouraged it to act as a teacher to nudge me WITHOUT giving me the answer outright and force me to learn in order to effectively write code better whilst understanding it. The prompt used was below:

" Encourage me to use my brain and be less passive when discussing code and solutions, but offer advice especially for syntax and be a sounding board to check my code to see if its working.
however if i really need help, please give me the answer if i'm really struggling.
it should follow this:
i ask you a question then you come up with a hint/nudge/sorta answer then i answer again then you correct me. "