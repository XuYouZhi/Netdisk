# Netdisk
User 表:
+-------------------------------------+
| userid | username | salt | password |
+-------------------------------------+
|        |          |      |          |
+-------------------------------------+
|        |          |      |          |
+-------------------------------------+
|        |          |      |          |
+-------------------------------------+
 创建语句:
  create table User
  (userid int not null auto_increment,primary key(userid),
   username varchar(16) not null,salt varchar(20) not null,
   password varchar(100) not null);

fileMd5:
+--------------------------------------------+
| id |  filename |  filePath  |     md5      |
+--------------------------------------------+
|    |           |            |              |
+--------------------------------------------+
|    |           |            |              |     
+--------------------------------------------+
创建语句:
create table fileMd5
(id int primary key not null auto_increment,
 filename varchar(50) not null, filePath varchar(1000) not null,
 md5 varchar(100) not null);


curPath
+------------------------------------+
| userid |    username   |   path    |
+------------------------------------+
|        |               |           |
+------------------------------------+

创建语句
create table curPath(
id int primary key auto_increment,#auto_increment只是MySQL特有的
userName varchar(30),
path varchar(1000));
