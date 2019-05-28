#pragma once

// Created by Nathan Dunne
// A class to handle the various calls to the Database

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <string>
#include <iostream>

#include "GameObject.h"

class DatabaseManager
{
public:

	static DatabaseManager* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new DatabaseManager();
			return s_pInstance;
		}
		return s_pInstance;
	}

	void runInitialScript();   //provides the initial setup of the database

	int getAttribute(std::string object, std::string attributeName);   // a function to retrieve values from the database

	std::vector<int> getScores();

	void changeAttribute(std::string objectName, std::string attributeName, int amountToChange);   // a function to change a value thats in the database
	void addScore(int score);

private:

	sql::mysql::MySQL_Driver* m_pDriver;
	sql::Connection* m_pCon;
	sql::Statement* m_pStmt;
	sql::ResultSet* m_pRes;

	void connect();//a function responsible for opening a connection to the database
	void disconnect();//responsible for closeing the connection and freeing any resources that were used

	static DatabaseManager* s_pInstance;
	DatabaseManager();
	~DatabaseManager();
};

typedef DatabaseManager TheDatabaseManager;

#endif
