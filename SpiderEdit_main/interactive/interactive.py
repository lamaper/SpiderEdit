#encoding=utf-8
# 1018

# import opt_database

import sqlite3

def dict_factory(cursor, row):  
    d = {}  
    for idx, col in enumerate(cursor.description):  
        d[col[0]] = row[idx]  
    return d  

def Connect(pos):
    connect = None
    try:
        connect = sqlite3.connect(pos)
        connect.row_factory = dict_factory
    except:
        return -33
    if connect is not None:
        return connect

def Close(cur, conn):
    if cur is not None:
        cur.close()
    if conn is not None:
        conn.close()

def Is_connectable(pos):
    connect = Connect(pos)
    
    try:
        if connect == -33:
            return False
        else:
            if connect is not None:
                connect.close()
            return True
    except:
        return False

def create_ind( pos ):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    try:
        cursor.execute(
            'CREATE TABLE DataBase'
            '(id          INTEGER PRIMARY KEY AUTOINCREMENT,'
            'dbnm         TEXT,'
            'clssnm       TEXT)'
        )
        connect.commit()
        Close(cursor, connect)
        return 1
    except:
        Close(cursor, connect)
        return 0

def create_db(pos):    
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    
    try:
        # 实现创建 EXAM 表，记录每一次考试的编号，考试时间，考试名称
        cursor.execute(
            'CREATE TABLE EXAM'
            '(id        INTEGER PRIMARY KEY AUTOINCREMENT,'
            'time       TEXT,'  # time: 日期。格式：YYYY-MM-DD
            'des        TEXT)'
        )
        # 实现创建 STUDENT 表，记录每一个学生的学号，姓名
        cursor.execute(
            'CREATE TABLE STUDENT'
            '(id        TEXT,'
            'name       TEXT)'
        )
        # 实现创建 KNOWLEDGE 表，记录第 i 个编号对应的考点i
        cursor.execute(
            'CREATE TABLE KNOWLEDGE'
            '(id        INTEGER PRIMARY KEY AUTOINCREMENT,'
            'content    TEXT)'
        )
        # 实现创建 WEAK 表,记录每个学生不会的知识点
        cursor.execute(
            'CREATE TABLE WEAK'
            '(stu_id      TEXT,'
            'exam_id      TEXT,'
            'knowledge_id TEXT)'
        )
        # 实现创建 exam-id 表,记录每次考试每个题目对应的知识点
        cursor.execute(
            'CREATE TABLE EAXM_ID'
            '(task_id      TEXT,'
            'knowledge_id  TEXT)'
        )
        connect.commit()
        Close(cursor, connect)
        return 1
    except:
        Close(cursor, connect)
        return 0


# 支持在任意表中插入一行并初始化
# e.g.: Insert_Init('/mnt/d/User_1st/project/Spider/test.db', 'EXAM', 20220920, '数学第一次练考')
def Insert_Init(pos, tablename, *params):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()

    a = []
    for i in params:
        a.append(i)
    data = tuple(a)
    
    res = 0
    if tablename == 'DataBase':
        try:
            cursor.execute("INSERT INTO DataBase (dbnm,clssnm) VALUES (?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -32          
    elif tablename == 'EXAM':
        try:
            cursor.execute("INSERT INTO EXAM (time,des) VALUES (?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -34
    elif tablename == 'STUDENT':
        try:
            cursor.execute("INSERT INTO STUDENT (id,name) VALUES (?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -35   
    elif tablename == 'KNOWLEDGE':
        try:
            cursor.execute("INSERT INTO KNOWLEDGE (id,content) VALUES (?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -36
    elif tablename == 'EAXM_ID':
        try:
            cursor.execute("INSERT INTO EAXM_ID (task_id,knowledge_id) VALUES (?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -37
    elif tablename == 'WEAK':
        try:
            cursor.execute("INSERT INTO WEAK (stu_id,exam_id,knowledge_id) VALUES (?,?,?)",data)
            connect.commit()
            res = 1
        except:
            res = -38
    else:
        res = -3
    Close(cursor, connect)
    return res

# 返回 table 表的行总数
# e.g.: TableRows('/mnt/d/User_1st/project/Spider/test.db', 'EXAM')
def TableRows(pos, table):
    res = 0
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    
    try:
        cursor.execute(f"select count(*) from {table}")
        res = cursor.fetchone()['count(*)']
    except:
        res = -3
    Close(cursor, connect)
    return res
    # print cur.fetchone()["a"]

# 查询 table 表中第 row 行第 col 列
# 使用前应先用 Isset 函数查询是否存在' 
# e.g.: Select_row_col('/mnt/d/User_1st/project/Spider/test.db', 'EXAM', 1, 'des')
def Select_row_col(pos, table, row, col):
    connect = Connect(pos)
    if connect == -33:
        return "*33"
    cursor = connect.cursor()
    
    assert type(table) == str
    assert type(row) == int
    assert type(col) == str

    try:
        cursor.execute(f"select * from {table} limit 1 offset {row-1};")
        res = cursor.fetchone()[col]
        Close(cursor, connect)
        return str(res)
    except:
        Close(cursor, connect)
        return '*'
    

# 查询 table 表中满足条件 where 的 col 列

def Select_col_where(pos, table, col, where):
    #return pos+'\n'+table+'\n'+col+'\n'+where
    connect = Connect(pos)
    if connect == -33:
        return '*33'
    cursor = connect.cursor()

    assert type(table) == str and type(col) == str and type(where) == str
    
    try:
        cursor.execute(f"select {col} from {table} where {where}")
        res = cursor.fetchone()[col]
        Close(cursor, connect)
        return str(res)
    except:
        Close(cursor, connect)
        return '*' 

# 判断是否存在 table 表中以及是否存在 col 列
# e.g.: Isset('/mnt/d/User_1st/project/Spider/test.db', 'EXAM', 'id')
# def Isset(pos, table, col):
#     connect = Connect(pos)
#     if connect == -33:
#         return -33
#     cursor = connect.cursor()

#     assert type(table) == str and type(col) == str

#     try:
#         cursor.execute("SELECT count(*) FROM sqlite_master WHERE type='table' AND name = '" + table + "'")
#         if cursor.fetchone()['count(*)'] == 0:
#             Close(cursor, connect)
#             return False
#         cursor.execute(f"select * from {table}")
#         d = cursor.fetchone()
#         if d.__contains__(col) == False:
#             Close(cursor, connect)
#             return False
#         else:
#             Close(cursor, connect)
#             return True
#     except:
#         Close(cursor, connect)
#         return -39
def Isset(pos, table, col):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    assert type(table) == str and type(col) == str
    try:
        cursor.execute("SELECT count(*) FROM sqlite_master WHERE type='table' AND name = '" + table + "'")
        if cursor.fetchone()['count(*)'] == 0:
            Close(cursor, connect)
            return False
        cursor.execute(f"select * from {table}")
        columns_tuple = cursor.description
        d = [field_tuple[0] for field_tuple in columns_tuple]
        if col not in d:
            Close(cursor, connect)
            return False
        else:
            Close(cursor, connect)
            return True
    except:
        Close(cursor, connect)
        return -39

# 修改 table 表 col 列满足条件 WHERE 的值为 content
# e.g.: Modify_col_where(pos, 'EXAM', 'time', "des='数学第一次练考'", 20221020)
def Modify_col_where(pos, table, col, where, content):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()

    assert type(table) == str and type(col) == str and type(where) == str
    sql = f'update {table} set {col}=? where {where}'
    data = tuple([content])
    try:
        cursor.execute(sql, data)
        connect.commit()
        Close(cursor, connect)
        return True
    except:
        Close(cursor, connect)
        return False

# 删除 table 表满足 where 的行
# e.g.: Delete_where(pos, 'EXAM', "des='数学第一次练考'")
def Delete_where(pos, table, where):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    last = 0
    res = []
    where += ' and'
    if 'and' in where:
        for j in range(0, len(where)):
            if where[j] == 'a' and where[j+1] == 'n' and where[j+2] == 'd':
                res.append(where[last:j-1])
                last = j + 4
    else:
        res.append(where)
    sql = f'delete from {table} where '
    data = ()
    for i in res:
        sql = sql + i[:i.index('=')] + '=? and '
        tmp = i[i.index('=')+1:]
        if tmp.isdigit():
            tmp = int(tmp)
        else:
            tmp = tmp[1:-1]
        data = data + (tmp,)
    sql = sql[:len(sql)-5]
    try:
        cursor.execute(sql, data)
        connect.commit()
        Close(cursor, connect)
        return True
    except:
        Close(cursor, connect)
        return False

def Check(pos, table):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    cursor.execute(f'select * from {table}')
    print(cursor.fetchall())
    Close(cursor, connect)

# 清空 table 表
def Delete_table(pos, table):
    connect = Connect(pos)
    if connect == -33:
        return -33
    cursor = connect.cursor()
    try:
        cursor.execute(f'drop table {table}')
        connect.commit()
        Close(cursor, connect)
        return True
    except:
        Close(cursor, connect)
        return False


# import readxls

import xlrd

def Open(pos):
    f = None
    try:
        f = xlrd.open_workbook(pos)
    except:
        return None
    if f != None:
        return f

def Query_col(filename, sheetname, col):
    f = Open(filename)
    if f == None:
        return []
    assert type(col) == str
    content = []
    try:
        a = f.sheet_names()
        if len(a) != 1:
            if sheetname not in a:
                f.release_resources()
                return []
            else:
                table=f.sheet_by_name(sheetname)
        else:
            if sheetname not in a:
                table = a[0]
            else:
                table=f.sheet_by_name(sheetname)
        R = table.nrows
        C = table.ncols
        frow = []
        for i in range(C):
            frow.append(table.col_values(i)[0])
        index = frow.index(col)
        for i in range(1, R):
            content.append( str(table.cell(i,index).value) )
        f.release_resources()
        return content
    except:
        f.release_resources()
        return []

