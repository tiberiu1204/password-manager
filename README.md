## Secure Password Manager
This is a secure password manager application. The cryptography algorithms are implemented by hand for didactic purposes. 
In other words, the manager is secure just in name, as there is the possibility of vulnerabilities in the code.

### Structure
The app consists of two cryptography algorithms:
- Bcrypt (used for password hashing)
- AES (used for user data encryption using the user's password as a key)

The app also has a CLI shell that the user can use to manage his account(s).
User accounts are stored in a local database, and their data is stored in individual files.

### Features
Currently, the app is in development. Current features include:
- Account creation
- User authentication
- Help menu
