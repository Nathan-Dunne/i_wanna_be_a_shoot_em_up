This game is built for Windows 10. 

The game requires a local instance of 32 bit MySQL server installed,
coupled with the 32 bit C++ connector.

https://dev.mysql.com/downloads/installer/ 	(32 bit 5.7.21 (Not the web community edition))

Using the full install option will install the 64 bit C++ connector.

The 32 bit C++ connector can then be downloaded and installed seperately. 

https://dev.mysql.com/downloads/connector/cpp/	 (32 bit 1.1.9)


The user should be root with a password of password.
The connection from the program to the database uses tcp://127.0.0.1:3306.


When confiugred, a new schema named "videogame" must be created in the MySQL workbench.

All other necessary SQL statements are handled internally within the program.

---------------------------------------------------------------------------------

While the executable can be run, given the above preconditions are satisfied, the Visual Studio
solution requires that libraries are placed in the correct locations.

These files cannot be added to the repository due to complications and size. To play the game you must run the executable in the Binary Folder.

----------------------------------------------------------------------------------

The game can be played with a controller or a keyboard, controls and instructions are provided in game.

Thank you for reading this, examining our submission and playing the game!

- The Team.


