#include "nrm.h"
#include "common.h"
#include "para.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
/*
 * main function of GEMF in C language
 * Futing Fan
 * Kansas State University
 * Last Modified: Jan 2016
 * Copyright (c) 2016, Futing Fan. All rights reserved. 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted
 */

/*
 * main 
 */
int _LOGLVL_;
void init_graph(Graph* graph, int echo);
void del_graph(Graph* graph);
void del_transition(Transition* tran);
void del_status(Status* sts);
void del_run(Run* run);
void load_graph(FILE* fil_para, Graph* graph);
void pre_init_graph(FILE* fil_para, Graph* graph);
void init_para(FILE* fil_para, Graph* graph, Transition* tran, Status* sts, Run* run, int echo);
void initi_status(FILE* fil_para, Graph* graph, Status* sts, int echo);
int main(int argc,char* argv[] ) {
    FILE* fil_para= NULL;
    int ret;
    int echo= 1;
    Graph graph;
    Transition tran;
    Status sts;
    Run run;

    _LOGLVL_= 0;
    if( argc< 2){
        fil_para= fopen( "para.txt", "r");
        if( fil_para== NULL){
            printf("cann't open default config file[para.txt], please check again\n");
            return -1;
        }
    }
    else{
        fil_para= fopen( argv[1], "r");
        if( fil_para== NULL){
            printf(" para file [%s] read error\n", argv[1]);
            return -1;
        }
        if( argc>2){
            if( !strcmp(argv[2], "DEBUG")){
                _LOGLVL_= 2;
            }
            else if( !strcmp(argv[2], "TRACE")){
                _LOGLVL_= 1;
            }
        }
    }


    //initialize running conditions
    init_para(fil_para, &graph, &tran, &sts, &run, echo);

    //pre initialize graph, basically all kinds of sizes
    pre_init_graph(fil_para, &graph);

    //initialize graph, memory allocation
    init_graph(&graph, echo);

    sts._node_s= graph._s;
    sts._node_e= graph._e;
    initi_status( fil_para, &graph, &sts, echo);
    //dump_status(&sts);

    load_graph(fil_para, &graph);

    //run simulation
    ret= nrm( &graph, &tran, &sts, &run);

    if( ret){
        printf("simulation error[%d]\n", ret);
    }
    else{
        printf("\nsimulation success!\n");
    }

    //clean up
    fclose(fil_para);
    del_graph(&graph);
    del_transition(&tran);
    del_status(&sts);
    del_run(&run);
    return 0;
}
void init_graph(Graph* graph, int echo){
    size_t memo_size, layer;
    LOG(1, __FILE__, __LINE__, "Init graph begin\n");
    graph->edge= NULL;
    graph->edge_w= NULL;
    graph->index= NULL;
    if( graph->weighted){
        graph->edge_w= (Edge_w**)malloc(sizeof(Edge_w*)*graph->L);
        if( graph->edge_w== NULL){
            printf("Memory allocation failure for network list, size[%zu]\n", sizeof(Edge_w*)*graph->L);
            exit( - 1);
        }
        for( layer= 0; layer< graph->L; layer++){
            memo_size= ((size_t)(2 - graph->directed))*(graph->E[layer]);
            if( memo_size< graph->E[layer]){
                printf(" Arithematic overflow layer[%zu], size [%d]*[%zu]\n", layer, 2 - graph->directed, graph->E[layer]);
                exit( - 1);
            }
            graph->edge_w[layer]= (Edge_w*)malloc(sizeof(Edge_w)*memo_size);
            if( graph->edge_w[layer]== NULL){
                printf("Memory allocation failure for layer[%zu], size[%zu]\n", layer, sizeof(Edge_w)*memo_size);
                exit( - 1);
            }
        }
    }
    else{
        graph->edge= (Edge**)malloc(sizeof(Edge*)*graph->L);
        if( graph->edge== NULL){
            printf("Memory allocation failure for network list, size[%zu]\n", sizeof(Edge*)*graph->L);
            exit( - 1);
        }
        for( layer= 0; layer< graph->L; layer++){
            memo_size= ((size_t)(2 - graph->directed))*(graph->E[layer]);
            if( memo_size< graph->E[layer]){
                printf(" Arithematic overflow layer[%zu], size [%d]*[%zu]\n", layer, 2 - graph->directed, graph->E[layer]);
                exit( - 1);
            }
            graph->edge[layer]= (Edge*)malloc(sizeof(Edge)*memo_size);
            if( graph->edge[layer]== NULL){
                printf("Memory allocation failure for layer[%zu], size[%zu]\n", layer, sizeof(Edge)*memo_size);
                exit( - 1);
            }
        }
    }
    LOG(1, __FILE__, __LINE__, "Init graph end\n");
    if(echo){
        print_graph_size(graph);
    }
}
void del_graph(Graph* graph){
    size_t layer;
    if( graph->edge!= NULL){
        for( layer= 0; layer< graph->L; layer++){
            if( graph->edge[layer]!= NULL){
                free(graph->edge[layer]);
            }
        }
        graph->edge= NULL;
    }
    if( graph->edge_w!= NULL){
        for( layer= 0; layer< graph->L; layer++){
            if( graph->edge_w[layer]!= NULL){
                free(graph->edge_w[layer]);
            }
        }
        graph->edge_w= NULL;
    }
    if( graph->E!= NULL){
        free( graph->E);
        graph->E= NULL;
    }
    if( graph->index!= NULL){
        for( layer= 0; layer< graph->L; layer++){
            if( graph->index[layer]!= NULL){
                free(graph->index[layer]);
            }
        }
        free( graph->index);
    }
}
void del_transition(Transition* tran){
    size_t i, layer;
    if( tran->nodal_trn!= NULL){
        for( i= tran->_s; i< tran->M+ tran->_s; i++){
            if( tran->nodal_trn[i]!= NULL){
                free( tran->nodal_trn[i]);
            }
        }
        free( tran->nodal_trn);
    }
    if( tran->edge_trn!= NULL){
        for( layer= 0; layer< tran->L; layer++){
            if( tran->edge_trn[layer]!= NULL){
                for( i= tran->_s; i< tran->M+ tran->_s; i++){
                    if( tran->edge_trn[layer][i]!= NULL){
                        free( tran->edge_trn[layer][i]);
                    }
                }
                free( tran->edge_trn[layer]);
            }
        }
        free( tran->edge_trn);
    }
    if( tran->inducer_lst!= NULL){
        free( tran->inducer_lst);
    }
}
void del_status(Status* sts){
    if( sts->init_lst!= NULL){
        free( sts->init_lst);
    }
    if( sts->init_cnt!= NULL){
        free( sts->init_cnt);
    }
}
void del_run(Run* run){
    if( run->out_file!= NULL) free (run->out_file);
}
void load_graph(FILE* fil_para, Graph* graph){
    printf("Reading network...\n");
    FILE* fil_dat= NULL;
    char* fil_nam= NULL;
    NINT tr, i, j;
    int ret;
    size_t li, layer;
    double t0= gettimenow();
    //read in network matrix [i j weight]
    locate_section( fil_para, "[DATA_FILE]");
    fil_nam= (char*)malloc(sizeof(char)*MAX_LINE_LEN);
    for(layer=0; layer< graph->L; layer++){
        LOG(2, __FILE__, __LINE__, "Read layer[%d]\n", layer+ 1);
        fget_next_item( fil_para, fil_nam, MAX_LINE_LEN);
        fil_dat= fopen( fil_nam, "r");
        if( fil_dat== NULL){
            printf("Read file[%s] error\n", fil_nam);
            exit( -1);
        }
        //skip comment lines on top
        skip_top_comment( fil_dat, '#');
        //weighted
        if( graph->weighted){
            LOG(2, __FILE__, __LINE__, "Read weighted network\n"); 
            for ( li= 0; li< graph->E[layer]; li++) {
                if (!(li % 10000000)&& li) {
                    time_print("[", gettimenow() - t0, " ]\t");
                    printf("layer[%zu] ", layer+ 1);
                    kilobit_print("[ ", (_LONG)li, "/");
                    kilobit_print("", (_LONG)graph->E[layer], " ] edges get\n");
                }
                ret= fscanf( fil_dat, fmt_n fmt_n " %lf", &i, &j, &(graph->edge_w[layer][li].w));
                if( ret != 3){
                    printf("Error! Expecting 3 columns, getting %d\n", ret);
                    exit(-1);
                }
                if( i< graph->_s || i> graph->_e){
                    printf("node["fmt_n"of layer[%zu]edge[%zu] out of range["fmt_n"/"fmt_n"]\n", i, layer, li, graph->_s, graph->_e);
                }
                if( j< graph->_s || j> graph->_e){
                    printf("node["fmt_n"of layer[%zu]edge[%zu] out of range["fmt_n"/"fmt_n"]\n", j, layer, li, graph->_s, graph->_e);
                }
                graph->edge_w[layer][li].i= i;
                graph->edge_w[layer][li].j= j;
                if( !graph->directed){
                    graph->edge_w[layer][graph->E[layer]+ li]= graph->edge_w[layer][li];
                    tr= graph->edge_w[layer][graph->E[layer]+ li].i;
                    graph->edge_w[layer][graph->E[layer]+ li].i= graph->edge_w[layer][graph->E[layer]+ li].j;
                    graph->edge_w[layer][graph->E[layer]+ li].j= tr;
                }
            }
        }
        //unweighted
        else{
            LOG(2, __FILE__, __LINE__, "Read unweighted network\n"); 
            for ( li= 0; li< graph->E[layer]; li++) {
                if (!(li % 10000000)&& li) {
                    time_print("[", gettimenow() - t0, " ]\t");
                    printf("layer[%zu] ", layer+ 1);
                    kilobit_print("[ ", (_LONG)li, "/");
                    kilobit_print("", (_LONG)graph->E[layer], " ] edges get\n");
                }
                ret= fscanf( fil_dat, fmt_n fmt_n , &i, &j);
                if( ret != 2){
                    printf("Error! Expecting 2 columns, getting %d\n", ret);
                    exit(-1);
                }
                if( i< graph->_s || i> graph->_e){
                    printf("node["fmt_n"of layer[%zu]edge[%zu] out of range["fmt_n"/"fmt_n"]\n", i, layer, li, graph->_s, graph->_e);
                }
                if( j< graph->_s || j> graph->_e){
                    printf("node["fmt_n"of layer[%zu]edge[%zu] out of range["fmt_n"/"fmt_n"]\n", j, layer, li, graph->_s, graph->_e);
                }
                graph->edge[layer][li].i= i;
                graph->edge[layer][li].j= j;
                if( !graph->directed){
                    graph->edge[layer][graph->E[layer]+ li]= graph->edge[layer][li];
                    tr= graph->edge[layer][graph->E[layer]+ li].i;
                    graph->edge[layer][graph->E[layer]+ li].i= graph->edge[layer][graph->E[layer]+ li].j;
                    graph->edge[layer][graph->E[layer]+ li].j= tr;
                }
            }
        }
        time_print("[", gettimenow() - t0, " ]\t");
        printf("layer[%zu] ", layer+ 1);
        kilobit_print("[ ", (_LONG)li, "/");
        kilobit_print("", (_LONG)graph->E[layer], " ] edges get\n");
        if( !graph->directed){
            graph->E[layer]+=  graph->E[layer];
        }

        fclose(fil_dat);
    }
    time_print( "initial time cost[ ", gettimenow() - t0, " ]\n");
    free(fil_nam);
}
void pre_init_graph(FILE* fil_para, Graph* graph){
    LINE str;
    size_t layer;
    _LONG val;
    //scan all network files, analysis metrics
    if( item_count( fil_para, "[NETWORK_INFO]")< (int)(2+ graph->L )){
        //missing NETWORK_INFO section or section incomplete, analysis from all network file
        printf("Analysis network info automaticly\n");
        locate_section( fil_para, "[DATA_FILE]");
        if( analysis_network( fil_para, graph)< 0)
            exit(-1);
    }
    else{
        locate_section( fil_para, "[NETWORK_INFO]");
        fget_next_item( fil_para, str, MAX_LINE_LEN);
        sscanf( str, "%d", &graph->weighted);
        fget_next_item( fil_para, str, MAX_LINE_LEN);
        sscanf( str, fmt_n" %lld", &graph->_s, &val);
        check_int_range( val);
        graph->V= (NINT)val - graph->_s+ 1;
        graph->_e= graph->_s+ graph->V;

        graph->E= (size_t*)malloc(sizeof(size_t)*graph->L);
        if( graph->E== NULL){
            printf("Memory allocation failure for edges number list, size[%zu]\n", sizeof(size_t)*graph->L);
            exit( - 1);
        }
        for( layer= 0; layer< graph->L; layer++){
            fget_next_item( fil_para, str, MAX_LINE_LEN);
            sscanf( str, "%zu", &graph->E[layer]);
            printf(" layer[%zu],", layer);
            kilobit_print(" [", (_LONG)graph->E[layer], " ]edges\n");
        }
    }
}
void init_para(FILE* fil_para, Graph* graph, Transition* tran, Status* sts, Run* run, int echo){
    //scan input file, analysis key parameter
    int ret;
    ret= item_count( fil_para, "[DATA_FILE]");
    if( ret<= 0){
        printf("wrong [DATA_FILE] config\n");
        exit( -1);
    }
    graph->L= (size_t)ret;

    tran->L= graph->L;
    tran->M= (size_t)item_count( fil_para, "[NODAL_TRAN_MATRIX]");
    sts->M= tran->M;
    printf("[compartment number]\t[%zu]\n", tran->M);
    printf("[layer number]\t\t[%zu]\n", tran->L);

    //read in status begin num
    sts->_s= (size_t)getValInt( fil_para, "[STATUS_BEGIN]", echo);
    tran->_s= sts->_s;

    //read in directed flag
    if( getValInt( fil_para, "[DIRECTED]", echo)> 0){
        graph->directed= 1; 
    }
    else{
        graph->directed= 0;
    }

    //read in output file names
    run->out_file= getValStr( fil_para, "[OUT_FILE]", MAX_LINE_LEN, echo);

    //read in max_time
    run->max_time= getValDbl( fil_para, "[MAX_TIME]", echo);

    //read in max events
    run->max_events= (size_t)getValInt( fil_para, "[MAX_EVENTS]", echo);

    //read in run times
    run->sim_rounds = (size_t)getValInt( fil_para, "[SIM_ROUNDS]", echo);

    //read in sample size
    run->interval_num = (size_t)getValInt( fil_para, "[INTERVAL_NUM]", echo);

    //print inducer for signle simulation if presented and non zero
    run->show_inducer= 0;
    if( item_count( fil_para, "[SHOW_INDUCER]")> 0){
        run->show_inducer = strcmp(getValStr( fil_para, "[SHOW_INDUCER]", MAX_LINE_LEN, echo), "0");
    }

    //read in inducer list
    tran->inducer_lst= getValSize_tLst( fil_para, "[INDUCER_LIST]", graph->L, echo);

    //read in nodal transition rate matrix
    tran->nodal_trn= getValMatrix( fil_para, "[NODAL_TRAN_MATRIX]", tran->M, tran->_s, MAX_LINE_LEN, echo);

    //read in edge based rate matrix
    tran->edge_trn= getValMatrixLst( fil_para, "[EDGED_TRAN_MATRIX]", tran->M, tran->L, tran->_s, MAX_LINE_LEN, echo);
}
void initi_status(FILE* fil_para, Graph* graph, Status* sts, int echo){
    char *fil_nam= NULL;
    FILE* fil_sts= NULL;
    size_t i;

    //read in status file
    LOG(2, __FILE__, __LINE__, "Read in status file\n");
    fil_nam= getValStr( fil_para, "[STATUS_FILE]", MAX_LINE_LEN, echo);
    fil_sts= fopen( fil_nam, "r");
    if( fil_sts== NULL){
        printf("Read file[%s] error\n", fil_nam);
        exit( -1);
    }
    if( initial_con( fil_sts, graph, sts)< 0){
        printf("initial status failed\n");
        exit(-1);
    }
    printf("[initial population]\t[");
    for( i= sts->_s; i< sts->M+ sts->_s; i++){
        kilobit_print(" ", (_LONG)sts->init_cnt[i], " ");
    }
    printf("]\n");
    fclose(fil_sts);
    LOG(2, __FILE__, __LINE__, "Read in status file success\n");
    if( fil_nam!= NULL) free (fil_nam);
}
