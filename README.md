# PasswordCrack
Password Cracking and Performance Measurement

# Packages Required for Execution

### Bither

#### Vcpkg
```
vcpkg install openssl:x64-windows 
vcpkg install sqlite3:x64-windows 
vcpkg integrate install
```
### Etherwall

#### Vcpkg
```
vcpkg install libsodium:x64-windows 
vcpkg install cryptopp:x64-windows 
vcpkg integrate install
```
### Sparrow

#### ODBC Driver

- **Version**: 9.0 (psqlodbc_09_00_0101-x64)
- **Download**: [PostgreSQL ODBC driver](https://www.postgresql.org/ftp/odbc/versions/msi/)

#### H2DB

- **Version**: 2.1.214
- **Note**: Ensure `h2-2.1.214.jar` is placed in the same path as the source code. For using the newest version, you might need to edit `server.bat`.
- **Download**: [H2DB](https://www.h2database.com/html/download.html)

#### Vcpkg
```
vcpkg install openssl
vcpkg install argon2
vcpkg integrate install
```

