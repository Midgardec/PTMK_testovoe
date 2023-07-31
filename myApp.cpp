#include <iostream>
#include "SQLiteCpp-master/include/SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp-master/include/SQLiteCpp/VariadicBind.h"
#include <chrono>

using namespace std;
using namespace SQLite;

// Функция для создания таблицы
void createTable(Database &db) {
    try {
        db.exec(R"(
            CREATE TABLE IF NOT EXISTS people (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                birth_date TEXT NOT NULL,
                gender TEXT NOT NULL
            )
        )");
        cout << "Table 'people' created successfully." << endl;
    } catch (Exception &e) {
        cerr << "Error creating table: " << e.what() << endl;
    }
}

// Функция для добавления записи
void insertRecord(Database &db, const string &name, const string &birthDate, const string &gender) {
    try {
        Statement query(db, "INSERT INTO people (name, birth_date, gender) VALUES (?, ?, ?)");
        query.bind(1, name);
        query.bind(2, birthDate);
        query.bind(3, gender);
        query.exec();
        cout << "Record added successfully." << endl;
    } catch (Exception &e) {
        cerr << "Error inserting record: " << e.what() << endl;
    }
}

// Функция для вывода всех строк с уникальным значением ФИО+дата и кол-вом полных лет
void displayUniqueRecords(Database &db) {
    try {
        Statement query(db, R"(
            SELECT name, birth_date, gender,
                (strftime('%Y', 'now') - strftime('%Y', birth_date)) - (strftime('%m-%d', 'now') < strftime('%m-%d', birth_date)) AS age
            FROM people
            GROUP BY name, birth_date
            ORDER BY name
        )");

        cout << "Unique records with age:" << endl;
        while (query.executeStep()) {
            cout << "Name: " << query.getColumn(0).getString()
                 << ", Birth Date: " << query.getColumn(1).getString()
                 << ", Gender: " << query.getColumn(2).getString()
                 << ", Age: " << query.getColumn(3).getInt() << endl;
        }
    } catch (Exception &e) {
        cerr << "Error displaying unique records: " << e.what() << endl;
    }
}

// Функция для заполнения автоматически 1000000 строк
void insertAutomaticRecords(Database &db) {
    try {
        db.exec("BEGIN;");
        for (int i = 0; i < 1000000; ++i) {
            string name = "Name" + to_string(i);
            string birthDate = "1990-01-01";
            string gender = (i % 2 == 0) ? "Male" : "Female";
            insertRecord(db, name, birthDate, gender);
        }
        db.exec("COMMIT;");
        cout << "1,000,000 records added successfully." << endl;
    } catch (Exception &e) {
        cerr << "Error inserting automatic records: " << e.what() << endl;
    }
}

// Функция для заполнения автоматически 100 строк с полом "Male" и ФИО начинающимся с "F"
void insertSpecialRecords(Database &db) {
    try {
        db.exec("BEGIN;");
        for (int i = 0; i < 100; ++i) {
            string name = "FamousName" + to_string(i);
            string birthDate = "1990-01-01";
            string gender = "Male";
            insertRecord(db, name, birthDate, gender);
        }
        db.exec("COMMIT;");
        cout << "100 special records added successfully." << endl;
    } catch (Exception &e) {
        cerr << "Error inserting special records: " << e.what() << endl;
    }
}

// Функция для выборки по критерию пол "Male" и ФИО начинается с "F"
void selectMaleNames(Database &db) {
    try {
        auto start = chrono::high_resolution_clock::now();
        Statement query(db, R"(
            SELECT name, birth_date, gender
            FROM people
            WHERE gender = 'Male' AND name LIKE 'F%'
        )");
        cout << "Results of the query:" << endl;
        while (query.executeStep()) {
            cout << "Name: " << query.getColumn(0).getString()
                 << ", Birth Date: " << query.getColumn(1).getString()
                 << ", Gender: " << query.getColumn(2).getString() << endl;
        }
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();
        cout << "Query execution time: " << duration << " ms" << endl;
    } catch (exception &e) {
        cerr << "Error executing query: " << e.what() << endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <operation>" << endl;
        return 1;
    }

    string databaseName = "database.db";
    Database db(databaseName, OPEN_READWRITE | OPEN_CREATE);

    int operation = stoi(argv[1]);

    switch (operation) {
        case 1:
            createTable(db);
            break;
        case 2:
            if (argc != 5) {
                cerr << "Usage: " << argv[0] << " 2 <name> <birth_date> <gender>" << endl;
                return 1;
            }
            insertRecord(db, argv[2], argv[3], argv[4]);
            break;
        case 3:
            displayUniqueRecords(db);
            break;
        case 4:
            insertAutomaticRecords(db);
            insertSpecialRecords(db);
            break;
        case 5:
            selectMaleNames(db);
            break;
        default:
            cerr << "Unknown operation." << endl;
            return 1;
    }

    return 0;
}
