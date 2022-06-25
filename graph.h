//
// Created by 孙发显 on 2022/6/3.
//

#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <sstream>
#include <string.h>
#include <fstream>
#include <string>
#include <vector>

struct inputListNode
{
    //数据流图中输入节点结构
    std::string block;//该输入来自哪个模块
    std::string inputName;//该输入变量名
};
struct edgeListNode
{
    //边表结构
    /*考虑到可能存在的边表结构扩展，故使用结构而非单个int变量*/
    int endNode;///边表单个元素，即边终点
};

class node
{
    //邻接表中单个元素的节点结构
private:
    std::string name;//节点名称，即变量名
    std::vector < std::string > dependList;//该变量的依赖变量，即图中的父节点，或者说表达式中的操作数，特别注意，包含常量
    int dependsNum;//依赖变量数量
    int OP_num;//操作码，详情参考paser.h

public:
    node ( )
    { };

    ~node ( )
    { };

    void set_name ( const std::string &_name )
    { name = _name; }//设置变量名
    void set_dependsNum ( const int _dependsNum )
    { dependsNum = _dependsNum; }//设置依赖变量数量
    void set_OP_num ( const int _OP_num )
    { OP_num = _OP_num; }//设置操作码
    void add_dependList ( const std::string &depend )
    { dependList.push_back ( depend ); }//增加依赖变量

    std::string get_name ( )
    { return name; }//设置变量名
    int get_dependsNum ( )
    { return dependsNum; }//入度
    int get_OP_num ( )
    { return OP_num; }//获取操作码
    bool get_dependList ( std::vector < std::string > &depends );//获取依赖变量列表
};

bool node::get_dependList ( std::vector < std::string > &depends )
{
    for ( int i = 0; i < dependList.size(); i ++ )
    {
        depends.push_back ( dependList[ i ] );
    }
    return true;
}

class listNode
{
//邻接表单个元素
private:
    node vertex;//节点
    std::vector < edgeListNode > edgeList;//该节点边表
public:
    listNode ( )
    { };

    ~listNode ( )
    { };
    int edgesNum;//出度
    void add_edgeList ( const edgeListNode &edgeListUnit )
    { edgeList.push_back ( edgeListUnit ); }//增加边表元素
    void get_edgeList ( std::vector < edgeListNode > &_edgeList );//获取边表
    node &get_vertex ( )
    { return vertex; }//获取节点信息
    void set_vertex ( const node &listUnit )
    { vertex = listUnit; }//设置节点

};

void listNode::get_edgeList ( std::vector < edgeListNode > &_edgeList )
{
    for ( int i = 0; i < edgeList.size ( ); i ++ )
    {
        _edgeList.push_back ( edgeList[ i ] );
    }
}


class block
{
    //数据块，从IR中截取单个模块的相关信息并存储
private:
    std::string label;//块名
    int nodeNum;//节点数（包含控制节点
    std::vector < node > nodeList;//节点列表

public:
    block ( std::ifstream &inf );

    block ( )
    { };

    ~block ( )
    { };

    void set_label ( const std::string &_label )
    { label = _label; }

    void set_nodeNum ( const int n )
    { nodeNum = n; }

    void add_nodeList ( const node &n )
    { nodeList.push_back ( n ); }

    std::string get_label ( )
    { return label; }

    int get_nodeNum ( )
    { return nodeNum; }

    bool get_nodeList ( std::vector < node > &nodes );
};

bool block::get_nodeList ( std::vector < node > &nodes )
{
    for ( int i = 0; i < nodeNum; i ++ )
    {
        nodes.push_back ( nodeList[ i ] );
    }
    return true;
}

block::block ( std::ifstream &inf )
{
    /*std::ifstream inf(filename);//打开文件
    if (!inf)
        exit(1);//打开失败返回1
         */
    std::string line;
    std::string key;
    char sep[] = " :";
    int block_flag = 0;
    int fileops;
    fileops = inf.tellg ( );
    while ( getline ( inf, line ))
    {
        char *tok;
        tok = strtok ( &line[ 0 ], sep );
        key = std::string ( tok );
        node temp_node;

        if ( key == "Basic" )
        {
            if ( block_flag == 0 )
            {
                block_flag = 1;
                tok = strtok ( NULL, sep );
                tok = strtok ( NULL, sep );
                tok = strtok ( NULL, sep );
                key = std::string ( tok );
                set_label ( key );
                nodeNum = 0;
            }
            else
            {
                inf.seekg ( fileops, std::ios::beg );
                return;
            }
        }
        else if ( key == "value" )
        {
            node temp_node;
            nodeNum ++;
            if (( tok = strtok ( NULL, sep )) != NULL )
            {
                key = std::string ( tok );
                temp_node.set_name ( key );
            }
            else
            {
                key = "";
                temp_node.set_name ( key );
            }

            getline ( inf, line );
            tok = strtok ( &line[ 0 ], sep );
            tok = strtok ( NULL, sep );
            tok = strtok ( NULL, sep );
            key = std::string ( tok );
            temp_node.set_OP_num ( std::stoi ( key ));

            getline ( inf, line );
            tok = strtok ( &line[ 0 ], sep );
            int count = 1;
            temp_node.add_dependList ( std::string ( tok ));
            while (( tok = strtok ( NULL, sep )) != NULL )
            {
                temp_node.add_dependList ( std::string ( tok ));
                count ++;
            }
            temp_node.set_dependsNum ( count );
            add_nodeList ( temp_node );
        }
        fileops = inf.tellg ( );
    }
}

class graph
{
    //数据流图
private:
    std::string name;//数据块名称
    std::vector < inputListNode > inputList;//输入节点列表
    std::vector < listNode > nodeList;//主体，在本数据块中被赋值的节点列表（或者说本数据块中产生的变量，采用邻接表存储
    node controlNode;//数据块的控制节点，br，return
public:
    graph ( block &basic_block );

    graph ( )
    { };

    ~graph ( )
    { };

    void set_inputListi_from(int i,const std::string & from)
    {
        inputList[i].block=from;
    }

    void add_nodeList ( const listNode &node )
    { nodeList.push_back ( node ); }//增加节点
    void add_inputList ( const inputListNode &nodeName )
    { inputList.push_back ( nodeName ); }//增加输入节点
    void set_name ( const std::string &_name )
    { name = _name; }//设置数据块名称，即label
    void set_controlNode ( const node &_controlNode )
    { controlNode = _controlNode; }//设置控制节点

    void get_nodeList ( std::vector < listNode > &_nodeList );//获取本数据块的变量列表
    void get_controlNode ( node &_controlNode )
    { _controlNode = controlNode; }//获取控制节点
    void get_inputList ( std::vector < inputListNode > &_inputList );//获取输入变量列表
    std::string get_name ( )
    { return name; }//获取模块名
};


void graph::get_nodeList ( std::vector < listNode > &_nodeList )
{
    for ( int i = 0; i < nodeList.size ( ); i ++ )
    {
        _nodeList.push_back ( nodeList[ i ] );
    }
}

void graph::get_inputList ( std::vector < inputListNode > &_inputList )
{
    for ( int i = 0; i < inputList.size ( ); i ++ )
    {
        _inputList.push_back ( inputList[ i ] );
    }
}

graph::graph ( block &basic_block )
{
    name = basic_block.get_label ( );
    int node_num = basic_block.get_nodeNum ( );
    int count = 0;
    std::vector < node > nodes;
    basic_block.get_nodeList ( nodes );

    for ( int i = 0; i < node_num; i ++ )
    {
        node node_i = nodes[ i ];
        //处理load
        //姑且认为数组仅在函数参数存在，为方便处理，所以不加入inputlist   6.10 zrz
        if(node_i.get_OP_num()==5)
        {
            listNode temp_listNode;
            temp_listNode.edgesNum = 0;
            temp_listNode.set_vertex ( node_i );
            add_nodeList ( temp_listNode );
            count++;
            continue;
        }

        //处理控制节点
        if ( nodes[ i ].get_OP_num ( ) == 7 )
        {
            //br
            set_controlNode ( nodes[ i ] );
            if(nodes[i].get_dependsNum()==3)
            {
                std::vector<std::string> deps;
                nodes[i].get_dependList (deps);
                int flag=0;
                for(int j=i-1;j>=0;j--)
                {
                    if(nodes[j].get_name()==deps[0])
                    {
                        flag=1;
                        break;
                    }
                }
                if(!flag)
                {
                    inputListNode temp;
                    temp.block="";
                    temp.inputName=deps[0];
                    add_inputList (temp);
                }
            }
            continue;
        }
        else if(nodes[ i ].get_OP_num ( ) == 14 )
        {
            set_controlNode ( nodes[ i ] );
            if(nodes[i].get_dependsNum()==1)
            {
                std::vector<std::string> deps;
                nodes[i].get_dependList (deps);
                int flag=0;
                for(int j=i-1;j>=0;j--)
                {
                    if(nodes[j].get_name()==deps[0])
                    {
                        flag=1;
                        break;
                    }
                }
                if(!flag)
                {
                    inputListNode temp;
                    temp.block="";
                    temp.inputName=deps[0];
                    add_inputList (temp);
                }
            }
            continue;
        }
        //正常计算结点
        listNode temp_listNode;
        temp_listNode.edgesNum = 0;
        if ( nodes[ i ].get_OP_num ( ) == 13 )
        {
            int num = nodes[ i ].get_dependsNum ( );
            node_i.set_dependsNum ( num / 2 );
        }
        temp_listNode.set_vertex ( node_i );
        add_nodeList ( temp_listNode );
        count ++;
        //处理phi
        if ( nodes[ i ].get_OP_num ( ) == 13 )
        {
            std::vector < std::string > depends;
            nodes[ i ].get_dependList ( depends );
            for ( int i = 0; i < depends.size ( ); i ++ )
            {
                if (( i % 2 ) == 0 )
                {
                    int flag = 0;
                    for ( int j = 0; j < inputList.size ( ); j ++ )
                    {
                        if ( depends[ i ] == inputList[ j ].inputName && depends[ i + 1 ] == inputList[ j ].block )
                        {
                            flag = 1;
                            break;
                        }
                    }
                    if ( ! flag )
                    {
                        inputListNode temp_inputNode;
                        temp_inputNode.inputName = depends[ i ];
                        temp_inputNode.block = depends[ i + 1 ];
                        add_inputList ( temp_inputNode );
                    }
                }
            }
            continue;
        }

        std::vector < std::string > depends;
        nodes[ i ].get_dependList ( depends );
        //遍历操作数，找到上家
        for ( int j = 0; j < nodes[ i ].get_dependsNum ( ); j ++ )
        {
            int flag = 0;
            for ( int k = 0; k < count; k ++ )
            {
                if ( nodeList[ k ].get_vertex ( ).get_name ( ) == depends[ j ] )
                {
                    edgeListNode temp;
                    temp.endNode = count - 1;
                    nodeList[ k ].add_edgeList ( temp );
                    nodeList[ k ].edgesNum ++;
                    flag = 1;
                    //假设满足SSA
                    break;
                }
            }
            if ( ! flag )
            {
                int i = 0;
                inputListNode temp_inputNode;
                for ( ; i < inputList.size ( ); i ++ )
                {
                    if ( inputList[ i ].inputName == depends[ j ] ) break;
                }
                if ( i == inputList.size ( ))
                {
                    temp_inputNode.block = "";
                    temp_inputNode.inputName = depends[ j ];
                    add_inputList ( temp_inputNode );
                }
            }

        }

    }
    //便利nodeList中所有的node

}

#endif //GRAPH_GRAPH_H
