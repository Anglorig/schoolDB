// MySQL Console Interface.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>

#include "mysql_connection.h"
#include "mysql_driver.h"
#include "mysql_error.h"
#include <cppconn\statement.h>
#include <cppconn\prepared_statement.h>
#include <cppconn\resultset.h>

/*Function prototypes*/
void searchQuery();						/*FUNCTION AUTHOR : IORDAN IVANOV 16*/
void updateMenu();						/*FUNCTION AUTHOR : IORDAN IVANOV 16*/
void updateTable(std::string table);	/*FUNCTION AUTHOR : IORDAN IVANOV 16*/
void printHelp();						/*FUNCTION AUTHOR : IORDAN IVANOV 16*/
void addStudent();						/*FUNCTION AUTHOR : STILIYAN HRISTOV 26*/
void addSubject();						/*FUNCTION AUTHOR : STILIYAN HRISTOV 26*/
void addGrade();						/*FUNCTION AUTHOR : STILIYAN HRISTOV 26*/
void deleteEntry();						/*FUNCTION AUTHOR : STILIYAN HRISTOV 26*/
void readDB();						    /*FUNCTION AUTHOR : STILIYAN HRISTOV 26*/
void setupDB();						    /*FUNCTION AUTHOR : TEODOR SPASOV 28*/
void mainMenu();						/*FUNCTION AUTHOR : TEODOR SPASOV 28*/
void conMenu();						    /*FUNCTION AUTHOR : TEODOR SPASOV 28*/

/*Global variables*/
sql::mysql::MySQL_Driver *driver;
sql::Connection *con;

int main()
{
	conMenu();
	mainMenu();

    return 0;
}

void setupDB()
{
	sql::Statement *stmt;
	try
	{
		stmt = con->createStatement();
		stmt->execute("CREATE DATABASE IF NOT EXISTS DBSCHOOL");
		con->setSchema("DBSCHOOL");

		stmt->execute("CREATE TABLE IF NOT EXISTS Students (Id INT NOT NULL AUTO_INCREMENT, Grade FLOAT NOT NULL, Class VARCHAR(1), Name VARCHAR(50), ClassNum INT NOT NULL, PRIMARY KEY (Id));");
		stmt->execute("CREATE TABLE IF NOT EXISTS Subjects (Id INT NOT NULL AUTO_INCREMENT, Name VARCHAR(50), TeacherName VARCHAR(50), PRIMARY KEY (Id));");
		stmt->execute("CREATE TABLE IF NOT EXISTS Grades (Id INT NOT NULL AUTO_INCREMENT, StudentId INT NOT NULL, SubjectId INT NOT NULL, Grade FLOAT NOT NULL, PRIMARY KEY (Id), FOREIGN KEY (StudentID) references Students(Id) ON DELETE CASCADE, FOREIGN KEY (SubjectId) references Subjects(Id) ON DELETE CASCADE);");
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	std::cout << "School database established!" << std::endl;
	delete stmt;
}

void mainMenu()
{
	int menuSelector = 0;
	char input = '0';

	SetConsoleCP(437);
	SetConsoleOutputCP(437);
	while (1)
	{
		input = 48;
		std::cout << (char)201;
		std::cout << std::string(30, (char)205);
		std::cout << (char)187 << std::endl;
		std::cout << (char)186 << ">1. Connect with the database." << (char)186 << std::endl;
		std::cout << (char)186 << ">2. Add a student.            " << (char)186 << std::endl;
		std::cout << (char)186 << ">3. Add a subject.            " << (char)186 << std::endl;
		std::cout << (char)186 << ">4. Add a grade.              " << (char)186 << std::endl;
		std::cout << (char)186 << ">5. View a table.             " << (char)186 << std::endl;
		std::cout << (char)186 << ">6. Search inside a table.    " << (char)186 << std::endl;
		std::cout << (char)186 << ">7  Update a table.           " << (char)186 << std::endl;
		std::cout << (char)186 << ">8. Delete an entry.          " << (char)186 << std::endl;
		std::cout << (char)186 << ">9. Help.                     " << (char)186 << std::endl;
		std::cout << (char)186 << ">E. Exit.                     " << (char)186 << std::endl;
		std::cout << (char)200;
		std::cout << std::string(30, (char)205);
		std::cout << (char)188 << std::endl;
		while ((input < 49 || input > 58) && input != 'E')
		{
			std::cout << (char)175;
			std::cin >> input;
		}
		switch (input)
		{
		case '1':
			setupDB();
			break;
		case '2':
			addStudent();
			break;
		case '3':
			addSubject();
			break;
		case '4':
			addGrade();
			break;
		case '5':
			readDB();
			break;
		case '6':
			searchQuery();
			break;
		case '7':
			updateMenu();
			break;
		case '8':
			deleteEntry();
			break;
		case '9':
			printHelp();
			break;
		case 'E':
			delete con;
			return;
		default:
			std::cout << "Invalid input!" << std::endl;
			break;
		}
	}
}

void conMenu()
{
	try {
		std::string host, username, password;
		std::cout << "Insert the IP and port of the MySQL server (press enter for default - tcp://127.0.0.1:3306/): " << std::endl;
		std::getline(std::cin, host);
		std::cout << "Insert your username: " << std::endl;
		std::getline(std::cin, username);
		std::cout << "Insert your password: " << std::endl;
		std::getline(std::cin, password);
		driver = sql::mysql::get_mysql_driver_instance();
		if (host == "") host = "tcp://127.0.0.1:3306/";
		con = driver->connect(host, username, password);
		if (con->isValid()) std::cout << "Connection established!" << std::endl;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

void addGrade()
{
	try {
		int studentId, subjectId;
		int validStudent = 0;
		int validSubject = 0;
		double grade;

		sql::PreparedStatement *prep_stmt;
		sql::Statement *stmt;
		sql::ResultSet *res;

		prep_stmt = con->prepareStatement("INSERT INTO GRADES(StudentId,SubjectId,Grade) VALUES(?,?,?)");
		stmt = con->createStatement();

		while (validStudent == 0) {
			std::cout << "Enter student Id: ";
			std::cin >> studentId;
			res = stmt->executeQuery("SELECT * FROM STUDENTS");
			while (res->next()) {
				if (res->getInt(1) == studentId) {
					validStudent = 1;
					break;
				}
			}
		}
		while (validSubject == 0) {
			std::cout << "Enter subject Id: ";
			std::cin >> subjectId;
			res = stmt->executeQuery("SELECT * FROM SUBJECTS");
			while (res->next()) {
				if (res->getInt(1) == subjectId) {
					validSubject = 1;
					break;
				}
			}
		}
		std::cout << "Enter student's grade: ";
		std::cin >> grade;


		prep_stmt->setInt(1, studentId);
		prep_stmt->setInt(2, subjectId);
		prep_stmt->setDouble(3, grade);

		prep_stmt->execute();

		std::cout << "Done" << std::endl;

		delete prep_stmt;
		delete stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
	std::cout << std::endl;	
}

void addStudent()
{
	try {
		int studentGrade, studentClassNum;
		std::string studentClass;
		std::string studentName;
		sql::PreparedStatement *prep_stmt;

		prep_stmt = con->prepareStatement("INSERT INTO STUDENTS(Grade,Class,Name, ClassNum) VALUES(?,?,?,?)");
		std::cout << "Insert the student's grade: ";
		std::cin >> studentGrade;
		std::cout << "Insert the student's class: ";
		std::cin.ignore();
		std::getline(std::cin, studentClass);
		std::cout << "Insert the student's name: ";
		std::getline(std::cin, studentName);
		std::cout << "Insert the student's number in the class: ";
		std::cin >> studentClassNum;
		std::cin.ignore();

		prep_stmt->setInt(1, studentGrade);
		prep_stmt->setString(2, studentClass);
		prep_stmt->setString(3, studentName);
		prep_stmt->setInt(4, studentClassNum);

		prep_stmt->execute();

		delete prep_stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}	
}

void addSubject()
{
	try {
		std::string subjectName;
		std::string subjectTeacherName;
		sql::PreparedStatement *prep_stmt;

		prep_stmt = con->prepareStatement("INSERT INTO SUBJECTS(Name, TeacherName) VALUES(?,?)");
		std::cin.ignore();
		std::cout << "Insert the subject's name: ";
		std::getline(std::cin, subjectName);
		std::cout << "Insert the subject's teacher name: ";
		std::getline(std::cin, subjectTeacherName);

		prep_stmt->setString(1, subjectName);
		prep_stmt->setString(2, subjectTeacherName);

		prep_stmt->execute();

		delete prep_stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}	
}

void readDB() {

	try {
		int choice;
		sql::Statement *stmt;
		sql::ResultSet *res;

		stmt = con->createStatement();
		std::cout << std::endl
			<< "1 - SHOW Students.\n"
			<< "2 - SHOW Grades\n"
			<< "3 - SHOW SUBJECTS\n"
			<< "4 - EXIT READ SELECTION\n"
			<< ">";
		std::cin >> choice;

		switch (choice) {
		case 1:
			res = stmt->executeQuery("SELECT * FROM STUDENTS");

			std::cout << "id | grade | class | name | classNum" << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getInt(2) << " | ";
				std::cout << res->getString(3) << " | ";
				std::cout << res->getString(4) << " | ";
				std::cout << res->getInt(5) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		case 2:
			res = stmt->executeQuery("SELECT * FROM GRADES");

			std::cout << "Id | StudentId | SubjectId | Grade" << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getInt(2) << " | ";
				std::cout << res->getInt(3) << " | ";
				std::cout << res->getDouble(4) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		case 3:
			res = stmt->executeQuery("SELECT * FROM SUBJECTS");

			std::cout << "Id | SubjectName | TeacherName " << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getString(2) << " | ";
				std::cout << res->getString(3) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		}
		delete res;
		delete stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

	std::cout << std::endl;
}

void updateMenu()
{
	char updateSelect = 48;

	while (1)
	{
		updateSelect = 48;
		std::cout << "Choose a table to update: " << std::endl;
		std::cout << "1. Students" << std::endl;
		std::cout << "2. Subjects" << std::endl;
		std::cout << "3. Grades" << std::endl;
		std::cout << "4. Exit update menu" << std::endl;
		while (updateSelect < 49 || updateSelect > 52)
		{
			std::cout << ">";
			std::cin >> updateSelect;
		}

		switch (updateSelect)
		{
		case '1':
			updateTable("STUDENTS");
			break;
		case '2':
			updateTable("SUBJECTS");
			break;
		case '3':
			updateTable("GRADES");
			break;
		case '4':
			return;
		}
	}
}

void updateTable(std::string table)
{
	sql::Statement *stmt;
	sql::ResultSet *res;
	std::string SET, WHERE, query, temp;
	int countSET = 0;
	int countWHERE = 0;
	int flag = 1;
	char selector;

	try {
		query = "UPDATE " + table + " ";
		SET = "SET";
		WHERE = "WHERE";
		stmt = con->createStatement();
		res = stmt->executeQuery("SELECT `COLUMN_NAME` FROM `INFORMATION_SCHEMA`.`COLUMNS` WHERE `TABLE_SCHEMA`='DBSCHOOL' AND `TABLE_NAME`='" + table + "'");
		std::cout << "------------------------------------" << std::endl;
		std::cout << " | ";
		while (res->next()) 
		{
			std::cout << res->getString("COLUMN_NAME") << " | ";
		}
		std::cout << std::endl << "------------------------------------" << std::endl;
		std::cout << "(Input STOP to apply your changes)" << std::endl;
		std::cin.ignore();
		while (flag)
		{
			std::string colSelect;
			selector = 48;
			std::cout << "Select a column by its name: ";
			getline(std::cin, colSelect);
			if (colSelect == "STOP")
			{
				flag = 0;
			}
			else
			{
				std::cout << "1. Set a new value for this column" << std::endl;
				std::cout << "2. Apply a criteria with this column" << std::endl;
				while (selector < 49 || selector > 50)
				{
					std::cout << ">";
					std::cin >> selector;
				}
				switch (selector)
				{
				case '1':
					std::cout << "Please remember to encase non-numbers in single quotes (')!" << std::endl;
					if (countSET > 0) SET += ", ";
					std::cout << "Set the new value : ";
					std::cin.ignore();
					getline(std::cin, temp);
					SET += " " + colSelect + "=" + temp;
					countSET++;
					break;
				case '2':
					if (countWHERE > 0) WHERE += ", ";
					std::cout << "Set the new criteria : ";
					std::cin.ignore();
					getline(std::cin, temp);
					WHERE += " " + colSelect + "=" + temp;
					countWHERE++;
					break;
				}
			}
			std::cout << std::endl << "------------------------------------" << std::endl;
		}
		query += SET;
		if (countWHERE > 0) query += " " + WHERE;
		if (countSET == 0)
		{
			std::cout << "No changes made!" << std::endl;
			return;
		}
		stmt->executeQuery(query);
	}
	catch (sql::SQLException &e) {
		if (e.getErrorCode() > 0)
		{
			std::cout << "# ERR: " << e.what();
			std::cout << " (MySQL error code: " << e.getErrorCode();
			std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
		}
	}
	delete stmt;
}

void printHelp()
{
	std::cout << std::string(30, '#') << std::endl;
	std::cout << "School database manual - " << std::endl;
	std::cout << "When performing search or update operations," << std::endl;
	std::cout << "you will be asked to select columns. When a column is" << std::endl;
	std::cout << "selected, you can either perform the operation on it, or" << std::endl;
	std::cout << "select it as a criteria. When selected as a criteria, you can" << std::endl;
	std::cout << "set what value the criteria for this column will be. Using this" << std::endl;
	std::cout << "you can limit the operation to a specific part in the table." << std::endl;
	std::cout << std::string(30, '#') << std::endl;
}

void deleteEntry() {
	try
	{
		int choice, deletionId;
		sql::PreparedStatement *prep_stmt;

		std::cout << "Pick a table" << std::endl
			<< " 1 - Students\n"
			<< " 2 - Subjects\n"
			<< " 3 - Grades\n"
			<< " Enter your choice and press return: ";
		std::cin >> choice;
		switch (choice) {
		case 1:
			prep_stmt = con->prepareStatement("DELETE FROM STUDENTS WHERE id=?");
			std::cout << " Enter student's id";
			std::cin >> deletionId;
			prep_stmt->setInt(1, deletionId);
			prep_stmt->execute();
			break;
		case 2:
			prep_stmt = con->prepareStatement("DELETE FROM SUBJECTS WHERE id=?");
			std::cout << " Enter subject's id";
			std::cin >> deletionId;
			prep_stmt->setInt(1, deletionId);
			prep_stmt->execute();
			break;
		case 3:
			prep_stmt = con->prepareStatement("DELETE FROM GRADES WHERE id=?");
			std::cout << " Enter grade's id";
			std::cin >> deletionId;
			prep_stmt->setInt(1, deletionId);
			prep_stmt->execute();
			break;
		}

		delete prep_stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}
}

void searchQuery() {
	try {
		char choice;
		int flag = 1;
		int countCRITERIA = 0;
		sql::Statement *stmt;
		sql::ResultSet *res;
		std::string query, criteria, temp, table;

		query = "SELECT ";
		criteria = "WHERE";

		stmt = con->createStatement();
		choice = 48;
		std::cout << "Choose a table to search in: " << std::endl;
		std::cout << "1. Students" << std::endl;
		std::cout << "2. Subjects" << std::endl;
		std::cout << "3. Grades" << std::endl;
		std::cout << "4. Exit search menu" << std::endl;
		while (choice < 49 || choice > 52)
		{
			std::cout << ">";
			std::cin >> choice;
		}
		if (choice == '1') table = "STUDENTS";
		if (choice == '2') table = "SUBJECTS";
		if (choice == '3') table = "GRADES";
		if (choice == '4') return;

		res = stmt->executeQuery("SELECT `COLUMN_NAME` FROM `INFORMATION_SCHEMA`.`COLUMNS` WHERE `TABLE_SCHEMA`='DBSCHOOL' AND `TABLE_NAME`='" + table + "'");
		std::cout << "------------------------------------" << std::endl;
		std::cout << " | ";
		while (res->next())
		{
			std::cout << res->getString("COLUMN_NAME") << " | ";
		}
		std::cout << std::endl << "------------------------------------" << std::endl;
		std::cout << "(Input STOP to start the search)" << std::endl;
		std::cin.ignore();
		while(flag)
		{
			std::string colSelect;
			std::cout << "Select a column by its name: ";
			getline(std::cin, colSelect);
			if (colSelect == "STOP")
			{
				flag = 0;
			}
			else
			{
				if (countCRITERIA > 0) criteria += ", ";
				std::cout << "Please remember to encase non-numbers in single quotes (')!" << std::endl;
				std::cout << "Set the new criteria : ";
				getline(std::cin, temp);
				criteria += " " + colSelect + "=" + temp;
				countCRITERIA++;
			}
			std::cout << std::endl << "------------------------------------" << std::endl;
		}

		std::cout << "RESULTS: " << std::endl;
		switch (choice) {
		case '1':
			query += "* FROM STUDENTS";
			query += " " + criteria;
			res = stmt->executeQuery(query);

			std::cout << "id | grade | class | name | classNum" << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getInt(2) << " | ";
				std::cout << res->getString(3) << " | ";
				std::cout << res->getString(4) << " | ";
				std::cout << res->getInt(5) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		case '2':
			query += "* FROM SUBJECTS";
			query += " " + criteria;
			res = stmt->executeQuery(query);

			std::cout << "Id | SubjectName | TeacherName " << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getString(2) << " | ";
				std::cout << res->getString(3) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		case '3':
			query += "* FROM GRADES";
			query += " " + criteria;
			std::cout << query << std::endl;
			res = stmt->executeQuery(query);

			std::cout << "Id | StudentId | SubjectId | Grade" << std::endl;
			while (res->next()) {
				std::cout << "------------------------------------" << std::endl;
				std::cout << res->getInt(1) << " | ";
				std::cout << res->getInt(2) << " | ";
				std::cout << res->getInt(3) << " | ";
				std::cout << res->getDouble(4) << " | " << std::endl;
			}
			std::cout << std::endl;
			break;
		}
		delete res;
		delete stmt;
	}
	catch (sql::SQLException &e) {
		std::cout << "# ERR: " << e.what();
		std::cout << " (MySQL error code: " << e.getErrorCode();
		std::cout << ", SQLState: " << e.getSQLState() << " )" << std::endl;
	}

	std::cout << std::endl;
}