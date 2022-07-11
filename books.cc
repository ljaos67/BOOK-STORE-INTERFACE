// NAME: LEONART JAOS
// Z-ID: z1911688
// CSCI 466-0001
// ASSIGN 10 			
// DUE DATE: 3/26/2021
//
// ASSIGNMENT DETAILS: In this assignment, I am using the mariadb sql functions to access a database and select 
// functions. This assignment uses the 'henrybooks' table in the 'courses' database. Using the C++ interface, I am
// able to design a main menu infinite loop that access the database data. I can perform 3 specific functions from 
// the database that for 2 options, searches based on 2 differrent types of user input and 1 option that lists 
// specific data. The main menu runs indefinitely unless the menu option to exit is chosen.
// -----------------------------------------------------------------------------------------------------------------


// Below are the preprocessing directives.
// iostream for input/output streams, mariadb for sql functions, string and c string for string and character string
// functions, and iomanip for spacing and formattin. Also using the standard namespace. 
#include <iostream>
#include <mariadb/mysql.h>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

// Struct used to input login details for mariadb server
struct connection_details
{
	const char *server, *user, *password, *database;
};

// predefined MYSQL opject that uses struct connection_details to login to server and database
// outputs error for connection failure
MYSQL* mysql_connection_setup(struct connection_details mysql_details)
{
	MYSQL *connection = mysql_init(NULL); 
if(!mysql_real_connect(connection, mysql_details.server, mysql_details.user, mysql_details.password, mysql_details.database, 0, NULL, 0))
{
	cout << "Connection Error: " << mysql_error(connection) << endl;
	exit(1);
}
return connection;

}
// Function that streamlines query input to mariadb server, it returns the result of the query
MYSQL_RES* mysql_perform_query(MYSQL *connection, const char *sql_query){
	if(mysql_query(connection, sql_query))
	{
		cout << "MySQL Query Error: " << mysql_error(connection) << endl;
		exit(1);
	}
	return mysql_use_result(connection);

}
int main()
{
	// establishing MYSQL connection objects, result objects, and row objects to be parsed later
	MYSQL *con;
	MYSQL *con2;
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL_RES *res2;
	MYSQL_ROW row2;
	// initializing connection details with my login information 
	struct connection_details login;
	login.server = "courses";
	login.user = "HIDDEN";
	login.password = "HIDDEN";
	login.database = "henrybooks";
	// I establish two connections to the server because later I will be running a select statement while 
	// another select statement is running
	con = mysql_connection_setup(login);
	con2 = mysql_connection_setup(login);
// infiinite loop for menu 
int choice;
do
{
	// Menu interface 
	cout << "---------------HENRYBOOKS ONLINE------------------\n";
	cout << "--------------------------------------------------\n";
	cout << "--------------------------------------------------\n";
	cout << "-    MAIN MENU                                   -\n";
	cout << "-(1) BOOK LIST                                   -\n";
	cout << "-(2) AUTHOR SEARCH                               -\n";
        cout << "-(3) TITLE LIST                                  -\n";
	cout << "-(4) EXIT                                        -\n";
	cout << "--------------------------------------------------\n";
	cout << "--------------------------------------------------\n";
	// menu option select
	cin >> choice;
	// option for display of book list 
	if(choice == 1)
	{
		// Prints book list, their author and their price. Prints blank line for multiple authors
		int rcount = 0;
		// This temp string is used to compare results with previous results in case a book has more tham
		// one author
		string tmp[4];
		res = mysql_perform_query(con, "SELECT Title, AuthorFirst, AuthorLast, Price FROM Book, Author, Wrote WHERE Author.AuthorNum = Wrote.AuthorNum AND Wrote.BookCode = Book.BookCode ORDER BY Book.Title, Wrote.Sequence;");
		cout << "Title" << "                                     " << "Author                   " << "Price\n";
		cout << "========================================  " << "====================== " << "=======\n";
		// While there are row results, output the data
		// I appended a dollar sign to the price and I included a statement to compare previous row data
		// to print books where there are two authors
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			string v = "$";
			v.append(row[3]);
			if(rcount == 0)
			{
				cout << setw(40) << left << row[0] <<"  " << setw(10) << left << row[1] << setw(12) << left << row[2] << " " << setw(7) << right << v << endl;
			}
			else
			{
				if(row[0] == tmp[0])
				{
					cout <<"                                          " << setw(10) << left << row[1] << setw(12) << left << row[2] << endl;
				}
				else
				{
				cout << setw(40) << left << row[0] <<"  " << setw(10) << left << row[1] << setw(12) << left << row[2] << " " << setw(7) << right << v << endl;
				}
			}
			rcount++;
			tmp[0] = row[0];
			tmp[1] = row[1];
			tmp[2] = row[2];
			tmp[3] = row[3];
		}
	// clear the result from the result object
	mysql_free_result(res);
	}
	// selection if search by Author
	else if(choice == 2)
	{
		// asks user to input author name and performs select statement to find books that match
		string ans;
		cout << "Which Author? ";
		string q1 = "SELECT Book.BookCode, Title, AuthorLast, AuthorFirst, Price FROM Book, Author, Wrote WHERE Author.AuthorNum = Wrote.AuthorNum AND Wrote.BookCode = Book.BookCode HAVING Author.AuthorFirst = '";
		string q2 = "' OR Author.AuthorLast = '";
		string q3 = "';";
		cin >> ans;
		string q4 = q1 + ans + q2 + ans + q3;
		const char *c = q4.c_str();
		// convert string to char array for use in res
		res = mysql_perform_query(con,c);
		cout << "BookCode " << "Title" << "                                      " << "Author                   " << " Price\n";
		cout <<"======== " << "=========================================  " << "====================== " << " =======\n";
		// while there are results in the row print the book, author, and price that match the search
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			string r1;
			r1.append(row[2]);
			r1 = r1 + ", ";
			r1.append(row[3]);
			string r2 = "$";
			r2.append(row[4]);
			cout << setw(8) << right <<  row[0]<< " " << setw(40) << left << row[1] << "   " << setw(22) << left << r1 <<"  " << setw(7)<< right << r2 << endl;
			// second select statement to find # of copies, branc name and location for each result from
			// the first select statement
			string p1 = "SELECT OnHand, BranchName, BranchLocation FROM Inventory, Branch WHERE Inventory.BookCode = '";
			p1.append(row[0]);
			p1 = p1 + "' AND Inventory.BranchNum = Branch.BranchNum;";
			const char * p = p1.c_str();	
			res2 = mysql_perform_query(con2,p);
			while((row2 = mysql_fetch_row(res2)) != NULL)
			{
				cout << "         - " << setw(2) << left << row2[0] << "available at " << row2[1] << " (" << row2[2] << ")\n";
			}
			cout << "\n";
			mysql_free_result(res2);		
		}
		mysql_free_result(res);
	}
	// choice if search by Book title
	else if(choice == 3)
	{
		// asks user for book title
		// gets full input stream with newline as string delimiter
		string ans;
		cout << "Which book (title)? ";
		cin.ignore();
		getline(cin,ans);
		string q1 = "SELECT Book.BookCode, Title, AuthorLast, AuthorFirst, Price FROM Book, Author, Wrote WHERE Author.AuthorNum = Wrote.AuthorNum AND Wrote.BookCode = Book.BookCode AND Wrote.Sequence = '1' HAVING Book.Title = '";
		string q2 = "';";
		string q4 = q1 + ans + q2;
		const char *c = q4.c_str();
		// performs search with book title, outputs bookcode, title, author, and price
		res = mysql_perform_query(con,c);
		cout << "BookCode " << "Title" << "                                      " << "Author                   " << " Price\n";
		cout <<"======== " << "=========================================  " << "====================== " << " =======\n";
		// appends data and formats data for specified output until all results are printed from query
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			string r1;
			r1.append(row[2]);
			r1 = r1 + ", ";
			r1.append(row[3]);
			string r2 = "$";
			r2.append(row[4]);
			cout << setw(8) << right <<  row[0]<< " " << setw(40) << left << row[1] << "   " << setw(22) << left << r1 <<"  " << setw(7)<< right << r2 << endl;
			string p1 = "SELECT OnHand, BranchName, BranchLocation FROM Inventory, Branch WHERE Inventory.BookCode = '";
			p1.append(row[0]);
			p1 = p1 + "' AND Inventory.BranchNum = Branch.BranchNum;";
			const char * p = p1.c_str();
			// performs second query search to look for # of copies, branch, and location of each
			// book from the first search	
			res2 = mysql_perform_query(con2,p);
			while((row2 = mysql_fetch_row(res2)) != NULL)
			{
				cout << "         - " << setw(2) << left << row2[0] << "available at " << row2[1] << " (" << row2[2] << ")\n";
			}
			cout << "\n";
			mysql_free_result(res2);		
		}
		mysql_free_result(res);
	}
	else
	{
		choice = 4;
	}
}
while((choice >= 1)&&(choice < 4));
// closes both connections
mysql_close(con);
mysql_close(con2);

return 0;
}

