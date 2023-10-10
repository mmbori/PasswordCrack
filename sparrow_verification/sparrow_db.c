#include "sparrow_db.h"

//for debugging. print error
void print_error(SQLHANDLE handle, SQLSMALLINT type) {
    SQLWCHAR sqlstate[6];
    SQLWCHAR message[512];
    SQLINTEGER native_error;
    SQLSMALLINT length;

    int recNum = 1;

    while (SQLGetDiagRec(type, handle, recNum, sqlstate, &native_error, message, sizeof(message) / sizeof(SQLWCHAR), &length) != SQL_NO_DATA) {
        printf("error %d:\n", recNum);
        printf(" SQLState: %ls\n", sqlstate);
        printf(" NativeError: %d\n", native_error);
        printf(" Message: %ls\n", message);
        recNum++;
    }
}

//initialize environment
int initialize_environment(SQLHANDLE* env) {

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, env)) return -1;

    if (SQL_SUCCESS != SQLSetEnvAttr(*env, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0)) return -1;
    return 0;
}

//free connection
void free_connection(SQLHANDLE con) {
    SQLFreeHandle(SQL_HANDLE_DBC, con);
}

//try connect to db
int test_connection(SQLHANDLE env, const char* db_name, const char* pwd, SQLHANDLE* con) {
    SQLWCHAR connectionString[1024];

    swprintf(connectionString, 1024, L"DRIVER={PostgreSQL Unicode(x64)};SERVER=localhost;PORT=5435;DATABASE=%hs;UID=sa;PWD=%hs;", db_name, pwd);

    if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, env, con)) return -1;

    SQLRETURN ret = SQLDriverConnect(*con, NULL, connectionString, SQL_NTS, NULL, 0, NULL, SQL_DRIVER_NOPROMPT);
    if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) {
        free_connection(con);
        return 1;
    }
    else {
        return 0;
    }
}

//start h2db server
void start_server(char* db_name) {
    char cmd[512];
    sprintf_s(cmd, sizeof(cmd), "start \"\" server.bat %s", db_name);
    system(cmd);
}


