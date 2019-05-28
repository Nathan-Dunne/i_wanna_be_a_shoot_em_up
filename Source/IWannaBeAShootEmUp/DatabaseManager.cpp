
// Created by Nathan Dunne
// A class to handle the various calls to the Database

#include "DatabaseManager.h"

DatabaseManager* DatabaseManager::s_pInstance;

// a function to retrieve values from the database
int DatabaseManager::getAttribute(std::string objectName, std::string attributeName)
{
	int value = 0;

	try
	{
		connect();		// connect to the database

		//execute a query to get the attribute name from the table
		m_pRes = m_pStmt->executeQuery("SELECT " + attributeName + " FROM attribute WHERE entityName = '" + objectName + "'");

		// set value to the attribute retrieved from the database
		while (m_pRes->next())
		{
			value = m_pRes->getInt(attributeName);
		}

		disconnect();   //close the connection

		return value; 
	}
	// if something fails, close the connection
	catch (sql::SQLException& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;

		disconnect();

		return -1;
	}
}

std::vector<int> DatabaseManager::getScores()
{
	std::vector<int> scores;

	try
	{
		connect();		// connect to the database

		//execute a query to get the attribute name from the table
		m_pRes = m_pStmt->executeQuery("SELECT score FROM scores;");

		// set value to the attribute retrieved from the database
		while (m_pRes->next())
		{
			scores.push_back( m_pRes->getInt("score"));
		}

		disconnect();   //close the connection

		return scores;
	}
	// if something fails, close the connection
	catch (sql::SQLException& e)
	{
		std::cout << "ERROR in getScores: " << e.what() << std::endl;

		disconnect();

		return scores;
	}
}

// a function to change a value thats in the database
void DatabaseManager::changeAttribute(std::string objectName, std::string attributeName, int amount)
{
	std::string amountToChange = std::to_string(amount);

	try
	{
		connect();	// open connection

		//update the value in the database using the variable passed in
		m_pStmt->execute("UPDATE attribute SET " + attributeName + " = " + attributeName + " + " + amountToChange + " WHERE entityName = '" + objectName + "'");

		disconnect();	//close connection
	}
	// if something fails, close the connection
	catch (sql::SQLException& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;

		disconnect();
	}
}

void DatabaseManager::addScore(int score)
{
	std::string scoreStr = std::to_string(score);
	try
	{
		connect();	// open connection

		//insert the score
		m_pStmt->execute("INSERT INTO scores VALUES(DEFAULT, " + scoreStr + ");");

		disconnect();	//close connection
	}
	// if something fails, close the connection
	catch (sql::SQLException& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;

		disconnect();
	}

	
}

//provides the initial setup of the database
void DatabaseManager::runInitialScript()
{
	try
	{
		connect();	//open connection

		m_pStmt->execute("DROP TABLE IF EXISTS attribute;");

		//create an attribute table
		m_pStmt->execute("CREATE TABLE attribute( "
			"attributeID SMALLINT(5) NOT NULL auto_increment,"
			"entityName CHAR(128),"
			"health SMALLINT(5),"
			"movementSpeed SMALLINT(5),"
			"damage SMALLINT(5),"
			"firingRate SMALLINT(5),"
			"projectileSpeed SMALLINT(5),"
			"luck SMALLINT(5),"
			"PRIMARY KEY(attributeID) );");

		//create a score table
		m_pStmt->execute("CREATE TABLE IF NOT EXISTS scores( "
			"attributeID SMALLINT(5) NOT NULL auto_increment,"
			"score INT(30),"
			"PRIMARY KEY(attributeID) );");

		//insert the default values for the player and enemies into the table
		m_pStmt->execute("INSERT INTO attribute VALUES(DEFAULT, 'PLAYER', 1, 7, 1, 10, 20, 3); ");

		m_pStmt->execute("INSERT INTO attribute VALUES(DEFAULT, 'ENEMY', 4, 3, 3, 60, 10, 3); ");

		m_pStmt->execute("INSERT INTO attribute VALUES(DEFAULT, 'ASSASSIN', 4, 3, 3, 60, 10, 3); ");
		m_pStmt->execute("INSERT INTO attribute VALUES(DEFAULT, 'UNO', 4, 3, 3, 60, 10, 3); ");
		m_pStmt->execute("INSERT INTO attribute VALUES(DEFAULT, 'NEWBORN', 7, 10, 3, 60, 10, 3); ");

		disconnect(); // close connection
	}
	// if something fails, close the connection
	catch (sql::SQLException& e)
	{
		std::cout << "ERROR: " << e.what() << std::endl;

		disconnect();
	}
}

//a function responsible for opening a connection to the database
void DatabaseManager::connect()
{
	// create connection to the local server
	m_pCon = m_pDriver->connect("tcp://127.0.0.1:3306", "root", "password");

	// create a statement from the connection
	m_pStmt = m_pCon->createStatement();

	// Use the test database
	m_pStmt->execute("USE videogame");
}

//responsible for closeing the connection and freeing any resources that were used
void DatabaseManager::disconnect()
{
	if (m_pRes != NULL)
	{
		delete m_pRes;
		m_pRes = NULL;
	}

	if (m_pStmt != NULL)
	{
		delete m_pStmt;
		m_pStmt = NULL;
	}

	if (m_pCon != NULL)
	{
		m_pCon->close();

		delete m_pCon;
		m_pCon = NULL;
	}
}

// set up the driver
DatabaseManager::DatabaseManager()
{
	m_pDriver = sql::mysql::get_mysql_driver_instance();

	m_pCon = NULL;
	m_pStmt = NULL;
	m_pRes = NULL;
}

DatabaseManager::~DatabaseManager()
{
	
}