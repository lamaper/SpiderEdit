#ifndef INTERACTIVE_H
#define INTERACTIVE_H

#include <Python.h>

#include <QString>
#include <QMessageBox>
#include <QApplication>

#include <string>
using std::string;
#include <windows.h>

#include <QDebug>

namespace Interactive {

void ext( QString err )
{
    QMessageBox::critical(0,"Error",err,QMessageBox::Ok,0);

    if( Py_IsInitialized() )
        Py_Finalize();

    qApp->exit(0);
    return;
}

void msb( QString err )
{
    QMessageBox::critical(0,"Error",err,QMessageBox::Ok,0);
    return;
}

string kw = "*";
bool iskw( char ch )
{
    for( int i = 0; i < (int)kw.size(); ++i )
    {
        if( kw[i] == ch )
            return true;
    }
    return false;
}

string _pos = "";
QMap<QString,QStringList> _cols;
QStringList _tables;
void db_init( QString dbpos )
{
    _pos = dbpos.toStdString();

    _tables << "EXAM" << "STUDENT" << "KNOWLEDGE" << "WEAK" << "EAXM_ID";

    _cols["DataBase"] << "id" << "dbnm" << "clssnm";
    _cols["EXAM"] << "id" << "time" << "des";
    _cols["STUDENT"] << "id" << "name";
    _cols["KNOWLEDGE"] << "id" << "content";
    _cols["WEAK"] << "stu_id" << "exam_id" << "knowledge_id";
    _cols["EAXM_ID"] << "task_id" << "knowledge_id";

    return;
}

void py_close()
{
    if( !Py_IsInitialized() ) return;

    Py_Finalize();
    return;
}

PyObject* pFunc[11];

void py_init()
{
    if( Py_IsInitialized() ) return;

    Py_SetPythonHome((wchar_t *)(L"../release/Python38")); // TODO
    Py_Initialize();
    if (!Py_IsInitialized())
    {
        py_close();
        ext("Py_Initialize");
    }

    PyObject* pModule = PyImport_ImportModule("interactive");
    if( !pModule )
    {
        py_close();
        ext("pModule");
    }

    pFunc[0] = PyObject_GetAttrString(pModule,"create_ind");
    pFunc[1] = PyObject_GetAttrString(pModule,"create_db");
    pFunc[2] = PyObject_GetAttrString(pModule,"Insert_Init");
    pFunc[3] = PyObject_GetAttrString(pModule,"TableRows");
    pFunc[4] = PyObject_GetAttrString(pModule,"Select_row_col");
    pFunc[5] = PyObject_GetAttrString(pModule,"Select_col_where");
    pFunc[6] = PyObject_GetAttrString(pModule,"Isset");
    pFunc[7] = PyObject_GetAttrString(pModule,"Modify_col_where");
    pFunc[8] = PyObject_GetAttrString(pModule,"Delete_where");
    pFunc[9] = PyObject_GetAttrString(pModule,"Query_col");
    pFunc[10] = PyObject_GetAttrString(pModule,"Is_connectable");

    for( int i = 0; i <= 10; ++i )
    {
        if( pFunc[i] == NULL )
        {
            py_close();
            ext("pFunc");
        }
    }
    return;
}

int create_ind( string dbpos=_pos )
{
    PyObject* pPara = PyTuple_New(1);
    PyTuple_SetItem(pPara,0,Py_BuildValue("s",dbpos.c_str()));
    PyObject* Func = PyObject_CallObject(pFunc[0],pPara);

    if( !Func )
       return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int create_db( string dbpos=_pos )
{
    PyObject* pPara = PyTuple_New(1);
    PyTuple_SetItem(pPara,0,Py_BuildValue("s",dbpos.c_str()));
    PyObject* Func = PyObject_CallObject(pFunc[1],pPara);

    if( !Func )
       return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int Insert_Init( string table, QStringList value, string dbpos=_pos )
{

    string vl[value.size()];
    for( int i = 0; i < value.size(); ++i )
    {
        vl[i] = value[i].toStdString();

//        if( iskw(vl[i][0] ) )
//            return -2;
    }

    PyObject* pPara = PyTuple_New(value.size()+2);

    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    for( int i = 0; i < value.size(); ++i )
        PyTuple_SetItem( pPara, i+2, Py_BuildValue("s",vl[i].c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[2],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int TableRows( string table, string dbpos = _pos )
{
    PyObject* pPara = PyTuple_New(2);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[3],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

QString Select_col_row( string table, string col, int row, string dbpos = _pos )
{
    PyObject* pPara = PyTuple_New(4);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("i",row) );
    PyTuple_SetItem( pPara, 3, Py_BuildValue("s",col.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[4],pPara);

    if( !Func )
        return "*1";

    char* res;
    PyArg_Parse(Func,"s",&res);

    QString qstr;
    qstr = QString::fromUtf8(res);

    return qstr;
}

QString Select_col_where( string table, string col, string where, string dbpos = _pos )
{
    if( !Py_IsInitialized() )
    {
        qDebug() << -1;
        py_init();
    }


    PyObject* pPara = PyTuple_New(4);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("s",col.c_str()) );
    PyTuple_SetItem( pPara, 3, Py_BuildValue("s",where.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[5],pPara);

    if( !Func )
        return "*1";

    char* res;
    PyArg_Parse(Func,"s",&res);

    QString qstr;
    qstr = QString::fromUtf8(res);

    return qstr;
}

int Isset( string table, string col, string dbpos = _pos)
{
    PyObject* pPara = PyTuple_New(3);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("s",col.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[6],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int Modify_col_where( string table, string col, string where, string content, string dbpos = _pos)
{
    PyObject* pPara = PyTuple_New(5);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("s",col.c_str()) );
    PyTuple_SetItem( pPara, 3, Py_BuildValue("s",where.c_str()) );
    PyTuple_SetItem( pPara, 4, Py_BuildValue("s",content.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[7],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int Del_where( string table, string where, string dbpos = _pos )
{
    PyObject* pPara = PyTuple_New(3);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",dbpos.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",table.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("s",where.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[8],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

int Que_xls( string file, string sheet, string col, QStringList &content )
{
    PyObject* pPara = PyTuple_New(3);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",file.c_str()) );
    PyTuple_SetItem( pPara, 1, Py_BuildValue("s",sheet.c_str()) );
    PyTuple_SetItem( pPara, 2, Py_BuildValue("s",col.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[9],pPara);

    if( !Func )
        return -1;

    int res = PyList_Size(Func);
    char* tmp;
    PyObject *Item = NULL;
    for(int i = 0; i < res; ++i)
    {
        Item = PyList_GetItem(Func,i);

        if( !Item )
            return -1;

        PyArg_Parse(Item, "s", &tmp);
        content << QString::fromUtf8(tmp);
     }

    return res;
}

int check_db( string file )
{
    for( int i = 0; i < _tables.size(); ++i )
    {
        for( int j = 0; j < _cols[_tables[i]].size(); ++j )
        {
            int tmp = Isset( _tables[i].toStdString(), _cols[_tables[i]][j].toStdString(), file );

            if( tmp != 1 ) return tmp;
        }
    }
    return 1;
}

int Is_connectable( string file=_pos )
{
    PyObject* pPara = PyTuple_New(1);
    PyTuple_SetItem( pPara, 0, Py_BuildValue("s",file.c_str()) );

    PyObject* Func = PyObject_CallObject(pFunc[10],pPara);

    if( !Func )
        return -1;

    int res = -1;
    PyArg_Parse(Func,"i",&res);

    return res;
}

}

#endif // INTERACTIVE_H
