Two Connections application
==========================================================================================
This application will perform the following actions:
1. Login using two different sets of credentials.
2. Set one of sessions for NO Price Update
3. Open position for the specified instrument in two sessions.
4. Logout from both sessions.

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
username1 - your user name. Mandatory argument.
password1 - your password. Mandatory argument.
host1 - the URL of the first server. Mandatory argument.
connection1 - first connection name. Mandatory argument. For example, "Demo" or "Real".
username2 - your user name for second session. Differs from username1. Mandatory argument.
password2 - your password for second session. Mandatory argument.
host2 - the URL of the second server. Mandatory argument.
connection2 - second connection name. Mandatory argument. For example, "Demo" or "Real".
[dbID1] - the first database name. Optional argument.
Required only for users which have multiple database login.
If you don't have multiple database login, leave this argument as it is.
[dbID2] - the second database name. Optional argument.
Required only for users which have multiple database login.
If you don't have multiple database login, leave this argument as it is.
[PIN1] - your pin code for first account. Optional argument.
Required only for users who have a pin.
If a pin is not required, leave this argument as it is.
[PIN2] - your pin code for second account. Optional argument.
Required only for users who have a pin.
If a pin is not required, leave this argument as it is.
