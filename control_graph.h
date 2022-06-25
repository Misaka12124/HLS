//
// Created by 孙发显 on 2022/6/4.
//

#ifndef GRAPH_CONTROL_GRAPH_H
#define GRAPH_CONTROL_GRAPH_H

#include <string>
#include <vector>
#include "graph.h"
#include <string.h>

enum cond
{
    FALSE,
    TRUE,
};
struct par
{
    //函数参数结构
    int parType;//0:非数组，1：数组
    std::string parameter;//参数名
};
struct controlNode
{
    //控制节点
    int cond;//条件，0：flase，1：true，2：无条件
    std::string nextGraph;//控制边的终点
    int nextGraphnum;
};
struct graph_node
{
    //控制图中的点，即单个数据块
    //采取类邻接表存储
    graph module;//数据块
    std::vector < controlNode > controledge;//控制边表
    std::vector < std::string > outputList;//单个数据块输出变量表
};

class control_graph
{
    //控制流图
private:
    std::string name;//名字，即函数名
    int ret_type;//返回类型 1:int 0：void
    std::vector < par > parList;//参数列表
    std::vector < graph_node > moduleList;//模块列表
    std::string retPar;//返回变量 “”为void返回

    void update_outputList ( );//更新每个模块的输出变量列表

public:
    control_graph ( std::string filename );

    control_graph ( )
    { };

    ~control_graph ( )
    { };

    int get_ret_type ( )
    { return ret_type; }

    std::string get_name ( )
    { return name; }

    std::string get_retPar ( )
    { return retPar; }//按照个人理解，返回值要么为空要么为int，故无需vector
    void get_parList ( std::vector < par > &_parList );

    void get_moduleList ( std::vector < graph_node > &_moduleList );
};

void control_graph::update_outputList ( )
{
    for ( int i = 0; i < moduleList.size ( ); i ++ )
    {
        graph current_handle = moduleList[ i ].module;
        std::vector < inputListNode > inputList;
        current_handle.get_inputList ( inputList );

        for ( int j = 0; j < inputList.size ( ); j ++ )
        {
            std::string from_block = inputList[ j ].block;
            if ( from_block == "" )
            {
                int finded = 0;
                for ( int k = i; k >= 0; k -- )//遍历之前的数据块包括自己
                {
                    std::vector < listNode > temp_nodeList;
                    moduleList[ k ].module.get_nodeList ( temp_nodeList );
                    for ( int m = 0; m < temp_nodeList.size ( ); m ++ )
                    {
                        node temp = temp_nodeList[ m ].get_vertex ( );
                        if ( inputList[ j ].inputName == temp.get_name ( ))
                        {
                            std::string from_name=moduleList[k].module.get_name();
                            moduleList[i].module.set_inputListi_from (j,from_name);

                            int  chongfu=0;
                            for(int n=0;n<moduleList[k].outputList.size();n++)
                            {
                                if(moduleList[k].outputList[n]==inputList[j].inputName)
                                {
                                    chongfu=1;
                                    break;
                                }
                            }
                            if(chongfu==0)
                            {
                                moduleList[ k ].outputList.push_back ( inputList[ j ].inputName );
                            }
                            finded = 1;
                            break;
                        }
                    }
                    if ( finded == 1 ) break;
                }
            }
            else
            {
                int finded = 0;
                for ( int k = 0; k < moduleList.size ( ); k ++ )
                {
                    if ( from_block == moduleList[ k ].module.get_name ( ))
                    {
                        std::vector < listNode > temp_nodeList;
                        moduleList[ k ].module.get_nodeList ( temp_nodeList );
                        for ( int m = 0; m < temp_nodeList.size ( ); m ++ )
                        {
                            node temp = temp_nodeList[ m ].get_vertex ( );
                            if ( inputList[ j ].inputName == temp.get_name ( ))
                            {

                                int  chongfu=0;
                                for(int n=0;n<moduleList[k].outputList.size();n++)
                                {
                                    if(moduleList[k].outputList[n]==inputList[j].inputName)
                                    {
                                        chongfu=1;
                                        break;
                                    }
                                }
                                if(chongfu==0)
                                {
                                    moduleList[ k ].outputList.push_back ( inputList[ j ].inputName );
                                }
                                finded = 1;
                                break;
                            }
                        }
                    }
                    if ( finded == 1 ) break;
                }
            }
        }
        /*
         *来历：syj要求加入
         * 删除：6.10要求删除，因干涉位宽
         //当前模块跳转为有条件br
        node temp_control;
        moduleList[i].module.get_controlNode (temp_control);
        if(temp_control.get_dependsNum()==3)
        {
            std::vector<std::string> dependList_control;
            temp_control.get_dependList (dependList_control);
            std::string cond_name=dependList_control[0];

            int  chongfu=0;
            for(int n=0;n<moduleList[i].outputList.size();n++)
            {
                if(moduleList[i].outputList[n]==cond_name)
                {
                    chongfu=1;
                    break;
                }
            }
            if(chongfu==0)
            {
                moduleList[i].outputList.push_back (cond_name);
            }

        }*/

    }
}

void control_graph::get_parList ( std::vector < par > &_parList )
{
    for ( int i = 0; i < parList.size ( ); i ++ )
    {
        _parList.push_back ( parList[ i ] );
    }
}

void control_graph::get_moduleList ( std::vector < graph_node > &_moduleList )
{
    for ( int i = 0; i < moduleList.size ( ); i ++ )
    {
        _moduleList.push_back ( moduleList[ i ] );
    }
}

control_graph::control_graph ( std::string filename )
{
    std::ifstream inf ( filename, std::ios::binary );
    std::string line;
    char sep[] = " :";
    int extr_pos;
    while ( getline ( inf, line ))
    {
        char *tok;
        std::string key;
        tok = strtok ( &line[ 0 ], sep );
        key = std::string ( tok );
        if ( key == "ret" )
        {
            tok = strtok ( NULL, sep );
            tok = strtok ( NULL, sep );
            key = std::string ( tok );
            if ( key == "int" )
            {
                ret_type = 1;
            }
            else
            {
                ret_type = 0;
            }
        }
        else if ( key == "function" )
        {
            tok = strtok ( NULL, sep );
            tok = strtok ( NULL, sep );
            key = std::string ( tok );
            name = key;
            int count = 0;
            int inf_pos = inf.tellg ( );
            while ( getline ( inf, line ))
            {
                count ++;
                tok = strtok ( &line[ 0 ], sep );
                key = std::string ( tok );
                if ( key == "Basic" )
                {
                    inf.seekg ( inf_pos, std::ios::beg );
                    break;
                }

                if ( count % 2 == 1 )
                {
                    par temp;
                    if ( key == "array" )
                    {
                        temp.parType = 1;
                    }
                    else
                    {
                        temp.parType = 0;
                    }
                    getline ( inf, line );
                    count ++;
                    tok = strtok ( &line[ 0 ], sep );
                    key = std::string ( tok );
                    temp.parameter = key;
                    parList.push_back ( temp );
                    inf_pos = inf.tellg ( );
                }
            }
        }
        else
        {
            inf.seekg ( extr_pos, std::ios::beg );
            while ( ! inf.eof ( ))
            {
                block basic_block ( inf );
                graph basic_graph ( basic_block );
                graph_node temp;
                temp.module = basic_graph;

                node control;
                basic_graph.get_controlNode ( control );

                if ( control.get_OP_num ( ) == 7 )//br
                {
                    if ( control.get_dependsNum ( ) == 1 )
                    {
                        controlNode control_node;
                        control_node.cond = 2;
                        std::vector < std::string > depends_list;
                        control.get_dependList ( depends_list );
                        control_node.nextGraph = depends_list[ 0 ];
                        temp.controledge.push_back ( control_node );
                    }
                    else
                    {
                        std::vector < std::string > depends_list;
                        control.get_dependList ( depends_list );

                        controlNode control_node;

                        control_node.cond = 1;
                        control_node.nextGraph = depends_list[ 1 ];
                        temp.controledge.push_back ( control_node );

                        control_node.cond = 0;
                        control_node.nextGraph = depends_list[ 2 ];
                        temp.controledge.push_back ( control_node );
                    }

                    moduleList.push_back ( temp );
                }
                else if ( control.get_OP_num ( ) == 14 )
                {
                    controlNode control_node;
                    control_node.cond = 2;
                    std::vector < std::string > depends_list;
                    control.get_dependList ( depends_list );
                    control_node.nextGraph = depends_list[ 0 ];
                    temp.controledge.push_back ( control_node );
                    moduleList.push_back ( temp );
                    if ( ret_type )
                    {
                        retPar = depends_list[ 0 ];
                    }
                    break;
                }
                else
                {
                    moduleList.push_back ( temp );
                }
            }
        }
        extr_pos = inf.tellg ( );
    }

    //处理控制边
    for ( int i = 0; i < moduleList.size ( ) - 1; i ++ )
    {
        for ( int j = 0; j < moduleList[ i ].controledge.size ( ); j ++ )
        {
            std::string nextName = moduleList[ i ].controledge[ j ].nextGraph;
            if ( nextName != "" )
            {
                for ( int k = 0; k < moduleList.size ( ); k ++ )
                {
                    if ( nextName == moduleList[ k ].module.get_name ( ))
                    {
                        moduleList[ i ].controledge[ j ].nextGraphnum = k;
                        break;
                    }
                }
            }
        }
    }
    update_outputList ( );
}


#endif //GRAPH_CONTROL_GRAPH_H
