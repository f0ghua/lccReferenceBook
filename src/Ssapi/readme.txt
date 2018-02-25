SourceSafe script

SourceSafe script is a small program used for automating tasks for Visual SourceSafe 6.
Provide a filename as commandline parameter which contains commands to execute.

The format of a command is:
<COMMAND> (case insensitive) followed by zero or more comma's and their values.
You can put as many whitespaces as you want between the fields, but a line may only be as big as 512 letters.
A '#' (hash) sign as the first character means that the line is a comment.


example:
PROJECT, d:\dev\vss\srcsafe.ini, Servé Laurijssen, mypassword


The following commands are possible:

PROJECT
GET
CHECKOUT
CHECKIN

*********************************************************************************************************

PROJECT	,<projectfile ini>, <username>, <password>

The project command has to be executed before all others, otherwise the other commands will fail.
What it does is making a connection to a sourcesafe database with the supplied username and password.
The username and password can be empty, but the comma's must be there.

Examples:
PROJECT, d:\dev\vss\srcsafe.ini,,
PROJECT, d:\dev\vss\srcsafe.ini, Servé Laurijssen, mypassword


*********************************************************************************************************

GET			,<file or directory>

GET performs a recursive "get latest version" on a directory or it performs a regular "get" on one file.

Examples:
GET, $/directory/somefile.c
GET, $/directory
GET, $/

*********************************************************************************************************
CHECKOUT,<file or directory>

Checkout checks one file or a complete directory out. This is not done recursively.

Examples:
CHECKOUT,$/directory/somefile.c
CHECKOUT,$/directory

*********************************************************************************************************

CHECKIN	,<file or directory>

Checkout checks one file or a complete directory in. This is not done recursively.

Examples:
CHECKIN,$/directory/somefile.c
CHECKIN,$/directory
