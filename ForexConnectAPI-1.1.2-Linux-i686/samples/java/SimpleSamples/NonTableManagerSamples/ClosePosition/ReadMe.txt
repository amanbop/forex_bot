Close Position application
==========================================================================================
This application will perform the following actions:
1. Login.
2. Display list of open positions.
3. Close the specified position.
4. Logout.

Building the application
===========================================================================================
In order to build this application you will need Java SDK 1.6 and Apache Ant.
You can download Java SDK from http://www.oracle.com/technetwork/java/javase/downloads
You can download Apache Ant from http://ant.apache.org/bindownload.cgi

Point your command prompt to the current directory and type: 
ant rebuild
For example,
C:\Program Files\CandleWorks\ForexConnectAPI\samples\java\SimpleSamples\ocloseposition>ant rebuild
This will generate directory structure .\build\classes\login and place compiled files there.

Running the application
============================================================================================
Change the build.xml file by putting your information in the arguments section.
For example, if your user name is testuser change the line
<arg value="{USERNAME}"/> to <arg value="testuser"/>.
Point your command prompt to the current directory and type:
ant run
For example,
C:\Program Files\CandleWorks\ForexConnectAPI\samples\java\SimpleSamples\closeposition>ant run
This will run the application and display the output on your console.

Arguments
==============================================================================================
{TRADE} - position, which you want to close. Mandatory argument.
For example, 31311694. 
{USERNAME} - your user name. Mandatory argument.
{PASSWORD} - your password. Mandatory argument.
{URL} - the URL of the server. 
The URL must be a full URL, including the path to the host descriptor. Mandatory argument. 
For example, http://www.fxcorporate.com/Hosts.jsp
{CONNECTION} - connection name. Mandatory argument. For example, "Demo" or "Real".
{DBNAME} - database name. Optional argument. 
Required only for users which have multiple database login.
If you don't have multiple database login, leave this argument as it is.
{PIN} - your pin code. Optional argument. Required only for users who have assigned pin.
If pin is not required, leave this argument as it is.









