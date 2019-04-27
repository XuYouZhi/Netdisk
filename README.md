# Netdisk
User 表:
--------------------------------------
| userid | username | salt | password |
---------------------------------------
|        |          |      |          |
---------------------------------------
|        |          |      |          |
---------------------------------------
|        |          |      |          |
---------------------------------------
 创建语句:
  create table User
  (userid int not null auto_increment,primary key(userid),
   username varchar(16) not null,salt varchar(20) not null,
   password varchar(100) not null);

filemd5:
----------------------------------------
| fileID | filePath | linknumber | md5 |
----------------------------------------
|        |          |            |     |
----------------------------------------
|        |          |            |     |
----------------------------------------
|        |          |            |     |
----------------------------------------
创建语句:



CurPath
-------------------------------------
| userid |    username   |   path    |
-------------------------------------
 创建语句
create table curPath(
id int primary key auto_increment,#auto_increment只是MySQL特有的
userName varchar(30),
path varchar(1000));

