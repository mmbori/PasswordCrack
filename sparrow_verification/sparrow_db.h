#pragma once

#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>

void print_error(SQLHANDLE handle, SQLSMALLINT type);
int initialize_environment(SQLHANDLE* env);
void free_connection(SQLHANDLE con);
int test_connection(SQLHANDLE env, const char* db_name, const char* pwd, SQLHANDLE* con);
void start_server(char* db_name);