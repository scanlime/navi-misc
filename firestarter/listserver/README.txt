Firestarter List server voodoo

To get the listserver working appropriately:

1) Install mysql if you haven't already -- learn how to use it http://dev.mysql.com/doc/mysql/en
2) Create a new database, call it fslistdb (you will have to be root)
  2a) mysqladmin -u root -p create fslistdb
3) Create a listserver user and password
  3a) mysql -u root -p
  3b) >GRANT select,insert,delete,update on fslistdb.* TO 'someuser'@'localhost' IDENTIFIED BY 'somesecretpassword';
  3c) >FLUSH privileges;
  3d) >\q
4) Create the table structure for the listserv
  4a) mysql -u someuser -p fslistdb < fslist.mysql
5) The db should be ready to go.