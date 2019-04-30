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
#后续因为代码逻辑需要，我往 fileMd5表中新增加了一列，语句如下
#fileNum 的数值表示 指向此 md5码所对应的文件的链接数
alter table fileMd5 add column fileNum int default 0;
alter table fileMd5 drop column fileNum;    #我后来发现我快传部分是通过
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
