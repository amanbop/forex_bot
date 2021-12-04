OpenPosition application
==========================================================================================
This application will perform the following actions:
1. Login.
2. Open position.
3. Logout.

Building the application
===========================================================================================
Windows:
	To build this application, you will need MS Visual Studio 2005 or later.

	You can run fxbuild.bat (fxbuild64.bat for 64-bit version) or select "build" in MS Visual Studio.
	
Linux/MacOS:
	To build this application, you will need:
		gcc-4.1 or later
		g++-4.1 or later
		CMake 2.6 or later (use 2.6 for MacOS)
		
	Run fxbuild.sh to build application.

Running the application
============================================================================================
You can run this application from the bin directory.
In Windows you can run this application executing fxrun.bat
All arguments must be passed from the command line.
This will run the application and display the output in your console.

You can run the application with no arguments, this will show the application Help.

Arguments
==============================================================================================
user - your user name. Mandatory argument.
password - your password. Mandatory argument.
url - the URL of the server. 
The URL must be a full URL, including the path to the host descriptor. Mandatory argument. 
For example, http://www.fxcorporate.com/Hosts.jsp
connection - the connection name. Mandatory argument. For example, "Demo" or "Real".
[subsession] - the database name. Optional argument.
Required only for users which have multiple database login.
If you don't have multiple database login, leave this argument as it is.
[pin] - your pin code. Optional argument. Required only for users who have a pin.
If a pin is not required, leave this argument as it is.

