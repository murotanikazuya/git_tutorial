#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
#include <fstream>

#include "user_interface.h"
#include "servo_ui.h"
#include "hydra.h"
#include "robot_hydra_id.h"
#include "chydradatamanager.h"
#include "cthreaddata.h"

//#define EHA_ON 0x0101
//#define EHA_OFF 0x0100

//extern CHydraDataManager hydra_data;
static CthreadData* hydraData;

//extern joint_state_t  jnt_state[HYDRA_JNT_MAX];
//externn joint_cmd_t    jnt_cmd[HYDRA_JNT_MAX];
//extern eha_state_t    eha_state[EHA_MAX];
//extern eha_cmd_t      eha_cmd[EHA_MAX];
extern double all_joint_tau_fk[HYDRA_JNT_MAX];
extern double all_joint_pos_tgt[HYDRA_JNT_MAX];
extern double all_joint_tau_tgt[HYDRA_JNT_MAX];

extern double all_joint_initpos[HYDRA_JNT_MAX];
extern double all_joint_finalpos[HYDRA_JNT_MAX];

//extern double all_joint_ampl[HYDRA_JNT_MAX];
//extern double all_joint_freq_hz[HYDRA_JNT_MAX];
//extern double all_joint_phase[HYDRA_JNT_MAX];

extern bool   all_joint_servo_switch[HYDRA_JNT_MAX];
//bool   all_joint_servo_switch[HYDRA_JNT_MAX];
extern bool   arm_IK_switch[2];
//extern unsigned short all_eha_switch[EHA_MAX];//
//extern double all_eha_pos[EHA_MAX];
//extern unsigned int all_eha_rawpos[EHA_MAX];
//extern double all_eha_vel[EHA_MAX];
//extern double all_eha_tau[EHA_MAX];
//extern double all_eha_tau2[EHA_MAX];
//extern double all_eha_tau3[EHA_MAX];

//extern double  eef_to_print[6];
//extern double  eef_to_update[6];

extern int thread_end_flg;

//extern bool interp_run;
//extern bool grasp_run;
//extern int  interp_cnt;
//extern int  interp_length;
//extern bool interp_ready;
//extern bool filemotion_run;
extern bool debug_mode;

extern double **motion_data;
extern int      motion_length;
extern int      motion_line;
extern char     motion_name[256];

extern FILE* fp_log;

extern double all_joint_refpos_to_send[HYDRA_JNT_MAX];
bool all_joint_resv_to_send[HYDRA_JNT_MAX];

//static thread_data_t *thread_data;
//static CthreadData* thread_data;

static void init_curses(void);
static void print_row_joint(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end);
static void print_labels_joint(WINDOW *pLabelWnd);
static void print_menu_joint(WINDOW *pMenuWnd);
static void print_title_joint(WINDOW *pTitleWnd);
static void print_row_EHA(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end);
static void print_labels_EHA(WINDOW *pLabelWnd);
static void print_menu_EHA(WINDOW *pMenuWnd);
static void print_title_EHA(WINDOW *pTitleWnd);
static void print_row_CART(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end);
static void print_labels_CART(WINDOW *pLabelWnd);
static int  show_quit_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y);
static void show_help_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y);
static void turn_off_all_EHA(void);
static void turn_off_all_joint(void);
static int  reload_motion_file(char *filename);
static int  show_dialog_get_filename(WINDOW *pWnd, char *filename, int size_x, int scroll_panel_size_y);

const int label_x_pos[]={0, 23, 32, 41, 46,56,66,76}; //52, 57, 66, 80};

/*
enum E_VIEW_MODE {
    E_VIEW_JOINT,
    E_VIEW_EHA,
    E_VIEW_CART
};
*/
#define E_VIEW_JOINT 0
#define E_VIEW_EHA   1
#define E_VIEW_CART  2
#define E_VIEW_MAX   3

#define CART_MAX     6

void *servo_ui(void *param)
{  
    WINDOW *pWnd;
    WINDOW *pLabelWnd;
    WINDOW *pMenuWnd;
    WINDOW *pTitleWnd;

    int view_mode=0;
    int x=0,y=0;
    static int y_prev[E_VIEW_MAX];
    int key;

    int size_x, size_y;
    int scroll_panel_size_y;
    int scroll_panel_size_x;
    int jnt_start;
    int jnt_end;
    int loop;

    //thread_data = (thread_data_t*)param;
    hydraData = (CthreadData*)param;

    std::ofstream ofs("log_ui");

    init_curses( );
    getmaxyx(stdscr, size_y, size_x);
    scroll_panel_size_y = size_y-5;
    scroll_panel_size_x = size_x-2;

    pWnd      = subwin(stdscr,scroll_panel_size_y, scroll_panel_size_x, 2, 1);
    pLabelWnd = subwin(stdscr,2, scroll_panel_size_x,        0, 1);
    pMenuWnd  = subwin(stdscr,1, scroll_panel_size_x, size_y-1, 1);
    pTitleWnd = subwin(stdscr,2, size_x,              size_y-3, 0);
    wbkgd(pTitleWnd, COLOR_PAIR(2));

    jnt_start = y;

    switch(view_mode){
    case E_VIEW_JOINT:
        jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
        break;
    case E_VIEW_EHA:
        jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;
        break;
    case E_VIEW_CART:
        jnt_end   = (scroll_panel_size_y>CART_MAX)?CART_MAX:scroll_panel_size_y;
        break;
    default:
        break;
    }

    /*
    if(view_mode==E_VIEW_JOINT)
        jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
    else if(view_mode==E_VIEW_EHA)
        jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;
        */

    x = 1;

    //表示ループ  
    while(1){

        erase();
        /*
        if(view_mode==E_VIEW_JOINT) {
            print_labels_joint(pLabelWnd);
            print_row_joint(pWnd, y, x, jnt_start, jnt_end);
            print_menu_joint(pMenuWnd);
            print_title_joint(pTitleWnd);
        }
        else if(view_mode==E_VIEW_EHA) {
            print_labels_EHA(pLabelWnd);
            print_row_EHA(pWnd, y, x, jnt_start, jnt_end);
            print_menu_EHA(pMenuWnd);
            print_title_EHA(pTitleWnd);
        }
        */
        switch(view_mode){
        case E_VIEW_JOINT:
            print_labels_joint(pLabelWnd);
            print_row_joint(pWnd, y, x, jnt_start, jnt_end);
            print_menu_joint(pMenuWnd);
            print_title_joint(pTitleWnd);
            break;
        case E_VIEW_EHA:
            print_labels_EHA(pLabelWnd);
            print_row_EHA(pWnd, y, x, jnt_start, jnt_end);
            print_menu_EHA(pMenuWnd);
            print_title_EHA(pTitleWnd);
            break;
        case E_VIEW_CART:
            print_labels_CART(pLabelWnd);
            print_row_CART(pWnd,y,x,jnt_start,jnt_end);
            break;
        default:
            //print_labels_CART(pLabelWnd);
            //print_row_CART(pLabelWnd,y,x,jnt_start,jnt_end);
            break;
        }

        refresh();//表示更新

//---------------------------------入力キーの処理-------------------------------------------
        key = getch();
    
        //矢印キーの処理
        switch(key){
        case '\t':
            y_prev[view_mode] = y;
            view_mode++;
            view_mode %= E_VIEW_MAX;
            y = y_prev[view_mode];
            jnt_start = 0;
            switch(view_mode){
            case E_VIEW_JOINT:
                jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
                break;
            case E_VIEW_EHA:
                jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;
                break;
            case E_VIEW_CART:
                jnt_end   = (scroll_panel_size_y>CART_MAX)?CART_MAX:scroll_panel_size_y;
                break;
            default:
                break;
            }
            /*
            if(view_mode==E_VIEW_JOINT)
                jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
            else if(view_mode==E_VIEW_EHA)
                jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;
            */
            break;

        case KEY_UP:
            if(y==jnt_start) {
                if(jnt_start>0) {
                    jnt_start--;
                    jnt_end = jnt_start + scroll_panel_size_y;
                    switch(view_mode){
                    case E_VIEW_JOINT:
                        jnt_end = (jnt_end>HYDRA_JNT_MAX-1)?HYDRA_JNT_MAX-1:jnt_end;
                        break;
                    case E_VIEW_EHA:
                        jnt_end = (jnt_end>EHA_MAX-1)?EHA_MAX-1:jnt_end;
                        break;
                    case E_VIEW_CART:
                        jnt_end = (jnt_end>CART_MAX-1)?CART_MAX-1:jnt_end;
                    }
                    /*
                    if(view_mode==E_VIEW_JOINT)
                        jnt_end = (jnt_end>HYDRA_JNT_MAX-1)?HYDRA_JNT_MAX-1:jnt_end;
                    else if(view_mode==E_VIEW_EHA)
                        jnt_end = (jnt_end>EHA_MAX-1)?EHA_MAX-1:jnt_end;
                    */
                }
                y = jnt_start;
            }
            else
                y--;
            break;

        case KEY_DOWN:
            if(y==jnt_end-1) {
                switch(view_mode){
                case E_VIEW_JOINT:
                    if(jnt_end<HYDRA_JNT_MAX) {
                        jnt_end++;
                        jnt_start = jnt_end - scroll_panel_size_y;
                        jnt_start = (jnt_start<0)?0:jnt_start;
                    }
                    break;
                case E_VIEW_EHA:
                    if(jnt_end<EHA_MAX) {
                        jnt_end++;
                        jnt_start = jnt_end - scroll_panel_size_y;
                        jnt_start = (jnt_start<0)?0:jnt_start;
                    }
                    break;
                case E_VIEW_CART:
                    if(jnt_end<CART_MAX) {
                        jnt_end++;
                        jnt_start = jnt_end - scroll_panel_size_y;
                        jnt_start = (jnt_start<0)?0:jnt_start;
                    }
                    break;
                }
                /*
                if(view_mode==E_VIEW_JOINT) {
                    if(jnt_end<HYDRA_JNT_MAX) {
                        jnt_end++;
                        jnt_start = jnt_end - scroll_panel_size_y;
                        jnt_start = (jnt_start<0)?0:jnt_start;
                    }
                }
                else if(view_mode==E_VIEW_EHA) {
                    if(jnt_end<EHA_MAX) {
                        jnt_end++;
                        jnt_start = jnt_end - scroll_panel_size_y;
                        jnt_start = (jnt_start<0)?0:jnt_start;
                    }
                }
                */
                y = jnt_end-1;
            }
            else
                y++;
            break;

        case KEY_RIGHT:
            // x++;
            // x &= 0x1;
            if(view_mode==E_VIEW_CART){
                    //eef_to_update[y] += 0.001;
            }

            break;
        case KEY_LEFT :
            // x--;
            // x &= 0x1;
            if(view_mode==E_VIEW_CART){
                    //eef_to_update[y] -= 0.001;
            }
            break;
        case KEY_F(2):
        {
            int j;
    
            for(j=0; j<HYDRA_JNT_MAX; j++) {
                all_joint_servo_switch[j] = true;
                all_joint_resv_to_send[j] = true;
                // for(l=0; l<joint_to_EHA_power[j][0]; l++) {
                //     k = joint_to_EHA_power[j][l+1];
                //     pthread_mutex_lock( &(thread_data->mutex) );
                //     all_eha_switch[k] = (all_joint_servo_switch[j]==true)?EHA_ON:EHA_OFF;
                //     pthread_mutex_unlock( &(thread_data->mutex) );
                // }
            }
        }
        break;
        case 'o':
        {
            if(view_mode==E_VIEW_JOINT) {
                int j, k;
                bool new_state;
                if(all_joint_servo_switch[y]==false) {
                    if(hydraData->flags["interp_run"])
                        goto endo;
                }

                new_state = !all_joint_servo_switch[y];
                for(j=0; j<joint_to_joint_power[y][0]; j++) {
                    k = joint_to_joint_power[y][j+1];
//                    pthread_mutex_lock( &(thread_data->mutex) );
//                    hydra_data.GetJointCmdPtr(1)[k].DATA.pos_ref
//                            = hydra_data.GetJointStatePtr(0)[k].DATA.pos_act;
//                    pthread_mutex_unlock( &(thread_data->mutex) );
                    all_joint_servo_switch[k] = new_state;
                    all_joint_resv_to_send[k] =  true;
                }
            }
        endo:
            break;
        }

        case 'u':
        case '+':
            if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                goto endu;
            hydraData->flags["interp_ready"] = false;
            if(view_mode==E_VIEW_JOINT) {
                if(hydraData->jnt.ref_raw.enabled[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        hydraData->jnt.ref_raw.pos[y] += DEG2RAD(0.1);
                        //all_joint_tau_tgt[y] += 0.1;
                    }
                }
            }
        endu:
            break;

        case 'U':
            if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                break;
            hydraData->flags["interp_ready"] = false;
            if(view_mode==E_VIEW_JOINT) {
                if(!all_joint_resv_to_send[y]) {
                    hydraData->jnt.ref_raw.pos[y] += DEG2RAD(0.5);
//                    all_joint_tau_tgt[y] += 1;
                }
            }/*
            else if(view_mode==E_VIEW_EHA) {
                //all_eha_reftau[y]=all_eha_reftau[y]+30;
                all_eha_reftau[y]=all_eha_reftau[y]+200; //0.5A
                //all_eha_reftau[y]=all_eha_reftau[y]+30; //0.35MPa : 100N
            }*/
            break;

        case 'd':
        case '-':
            if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                break;
            hydraData->flags["interp_ready"] = false;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        hydraData->jnt.ref_raw.pos[y] -= DEG2RAD(0.1);
//                        all_joint_tau_tgt[y] -= 0.1;
                    }
                }
            }
            break;

        case 'D':
            if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                break;
            hydraData->flags["interp_ready"] = false;
            if(view_mode==E_VIEW_JOINT) {
                    if(!all_joint_resv_to_send[y]) {
                        hydraData->jnt.ref_raw.pos[y] -= DEG2RAD(0.5);
//                        all_joint_tau_tgt[y] -= 1;
                    }
            }
            break;
/*
        case 'A':
            if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        all_joint_ampl[y] = all_joint_ampl[y] + DEG2RAD(0.1);
                    }
                }
            }
            break;
        case 'a':
            if( (thread_data->interp_run!=false)||(thread_data->filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        all_joint_ampl[y] = all_joint_ampl[y] - DEG2RAD(0.1);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;

        case 'F':
            if( (thread_data->interp_run!=false)||(thread_data->filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        //all_joint_freq_hz[y] = all_joint_freq_hz[y] + 0.01;
                        all_joint_freq_hz[y] = all_joint_freq_hz[y] + 0.1;
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;
        case 'f':
            if( (thread_data->interp_run!=false)||(thread_data->filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        //all_joint_freq_hz[y] = (all_joint_freq_hz[y]>0.01) ? (all_joint_freq_hz[y]-0.01) : all_joint_freq_hz[y];
                        all_joint_freq_hz[y] = (all_joint_freq_hz[y]>0.1) ? (all_joint_freq_hz[y]-0.1) : all_joint_freq_hz[y];
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;
*/
        case ' ':
            if(view_mode==E_VIEW_JOINT) {
                if(x==1){
                    int j, k;
                    bool new_state;
                    if(!hydraData->jnt.ref_raw.enabled[y]) {
                        if(hydraData->flags["interp_run"])
                            goto endsp;
                    }

                    new_state = !hydraData->jnt.ref_raw.enabled[y];
                    for(j=0; j<joint_to_joint_power[y][0]; j++) {
                        k = joint_to_joint_power[y][j+1];

                        hydraData->jnt.ref_raw.pos[k] = hydraData->jnt.act.pos[k];

                        all_joint_servo_switch[k] = new_state;

                        all_joint_resv_to_send[k] =  true;
                    }
                }
            }
        endsp:
            break;
/*
        case '=':
            if(view_mode==E_VIEW_JOINT) {
//                pthread_mutex_lock( &(thread_data->mutex) );
//                hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
//                        = hydra_data.GetJointStatePtr(0)[y].DATA.pos_act;
                all_joint_refpos_to_send[y]
                        = hydraData->GetJointStatePtr(0)[y].DATA.pos_act;
//                pthread_mutex_unlock( &(thread_data->mutex) );
                all_joint_resv_to_send[y] = true;
            }
            break;
*/
        case 'x':
            if(hydraData->flags["interp_run"]) {
                hydraData->flags["interp_run"] = false;
                ofs << "interporation disabled" << std::endl;
            } else {
                hydraData->interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydraData->jnt.act.pos[loop];
                }
                hydraData->flags["interp_run"] = true;
                ofs << "interporation enabled" << std::endl;
            }
            break;
/*
        case 'g':
            //thread_data->interp_run = (thread_data->interp_run==1) ? 0 : 1;
            if(thread_data->grasp_run==true) {
                thread_data->grasp_run = false;
            } else {
                thread_data->interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref;
                    all_joint_finalpos[loop] = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref;
                }
                all_joint_finalpos[JOINT_HYDRA_RHAND_THB_FE] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_IDX] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_MID] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_RNG] = 0;

                all_joint_finalpos[JOINT_HYDRA_LHAND_THB_FE] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_IDX] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_MID] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_RNG] = 0;

                //thread_data->interp_run = true;
            }
            break;

        case 'G':
            //thread_data->interp_run = (thread_data->interp_run==1) ? 0 : 1;
            if(thread_data->grasp_run==true) {
                thread_data->grasp_run = false;
            } else {
                thread_data->interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref;
                    all_joint_finalpos[loop] = hydraData->GetJointCmdPtr(0)[loop].DATA.pos_ref;
                }
                all_joint_finalpos[JOINT_HYDRA_RHAND_THB_FE] = 3 * M_PI / 3.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_IDX] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_MID] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_RNG] = -3 * M_PI / 2.0;

                all_joint_finalpos[JOINT_HYDRA_LHAND_THB_FE] = 3 * M_PI / 3.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_IDX] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_MID] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_RNG] = -3 * M_PI / 2.0;

                thread_data->grasp_run = true;
            }
            break;
            */

        case 'r':
            if( (hydraData->flags["interp_ready"])&&(!hydraData->flags["filemotion_run"]) ) {
                hydraData->flags["filemotion_run"] = true;
                ofs << "file motion run enabled" << std::cout;
            } else {
                hydraData->flags["filemotion_run"] = false;
                ofs << "file motion run disabled" << std::cout;
            }
            break;
/*
        case 'l':
        {
            char filename[256];
            memset(filename, 0, 256);
            if(show_dialog_get_filename(pWnd, filename, size_x, scroll_panel_size_y)==0) {
                fprintf(fp_log, "%s\n", filename);
                fflush(fp_log);
                reload_motion_file(filename);
                strcpy(motion_name, filename);
                thread_data->interp_ready = false;
            }
            break;
        }
*/
/*
        case 'I':
            if(view_mode==E_VIEW_CART){
                for(int i=0;i<6;i++){
                    eef_to_update[i]=eef_to_print[i];
                }
                for(int i=0;i<2;i++){
                    arm_IK_switch[i] = true;
                }
            }
            break;
        case 'i':
            if(view_mode==E_VIEW_CART){
                for(int i=0;i<2;i++){
                    arm_IK_switch[i] = false;
                }
            }
            break;
*/
        case KEY_DC:
            //turn_off_all_EHA( );
            //turn_off_all_joint( );
            hydraData->SetAllJointServoSwitch(false);
            break;
/*
        case '?': // show help
            show_help_dialog(pWnd, size_x, scroll_panel_size_y);
            break;
*/
        
        case 'q': //qで終了
        {
            if(show_quit_dialog(pWnd, size_x, scroll_panel_size_y)<0)
                goto skip_quit;

            //turn_off_all_EHA( );
            //turn_off_all_joint( );
            hydraData->SetAllJointServoSwitch(false);

            thread_end_flg = 0;

			// 自身のプロセスにシグナルを送信する 20160401 okamoto
			raise(SIGTERM);
            goto loop_end;
        skip_quit:
            break;
        }

        case '\n': //Enterの押された時の処理
            if(view_mode==E_VIEW_JOINT) {
                int j, k;
                if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"] )
                    goto endcr;
                hydraData->flags["interp_ready"] = false;
                for(j=0; j<joint_to_joint_power[y][0]; j++) {
                    k = joint_to_joint_power[y][j+1];
                    hydraData->jnt.ref_raw.pos[k] = hydraData->jnt.act.pos[k];


                    //all_joint_tau_tgt[k] = 0;
                    hydraData->jnt.ref_raw.enabled[k] = (all_joint_servo_switch[k]==true)?1:0;
                    all_joint_resv_to_send[k] = false;
                }

            }
        endcr:
            break;

        case KEY_F(12):

            if(view_mode==E_VIEW_JOINT) {
                int j, k, l;

                if( hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
                    goto endf12;

                hydraData->flags["interp_ready"] = false;

                hydraData->jnt.ref_raw.pos = hydraData->jnt.act.pos;

                for(j=0; j<HYDRA_JNT_MAX; j++) {
                    //all_joint_tau_tgt[j] = 0;
                    //all_joint_pos_tgt[j] = all_joint_refpos_to_send[j];
                    hydraData->jnt.ref_raw.enabled[j] = (all_joint_servo_switch[j]==true)?1:0;
                    all_joint_resv_to_send[j] = false;
                }
            }
        endf12:
            break;
/*
        case 'w':
        {
            int   loop;
            FILE *fp;
            if( (fp=fopen("ehapos.dump","w")) == NULL ) {
                fprintf(stderr, "Cannot open file!\n");
                break;
            }
            for(loop=0; loop<EHA_MAX; loop++) {
                fprintf(fp, "%d, %08x\n", loop,
                        hydraData->GetEHAStatePtr(0)[loop].DATA.rawpos_act);
            }
            fclose(fp);
            break;
        }
        */

        default:
            break;
        }
        //-------------------入力キーの処理ここまで-------------------

        usleep(10000);
    }//whileループここまで
loop_end:
    endwin();
    return(0);
}

void init_curses(void) 
{
    //ncurses初期化の諸々
    initscr();
    noecho();
    cbreak();
    keypad(stdscr,TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_GREEN,   COLOR_BLACK);//選択中の行の色
    init_pair(2, COLOR_BLACK,   COLOR_WHITE);//カーソル位置の色
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK);// 送信待ち
    init_pair(4, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(5, COLOR_BLACK,   COLOR_GREEN);
    init_pair(6, COLOR_BLACK,   COLOR_MAGENTA);
    init_pair(7, COLOR_RED,     COLOR_BLACK);
    init_pair(8, COLOR_BLACK,   COLOR_RED);
    init_pair(9, COLOR_RED,     COLOR_WHITE);
    timeout(0);
}

void print_row_joint(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end)
{
    int i;
    int height =1;
    int x_offset = 1;
    int y_offset = 0;
    int sel_mode=0;
    wbkgd(pWnd, COLOR_PAIR(0));
    for(i=jnt_start; i<jnt_end; i++) {
        if(i==jnt_sel) {
            if(all_joint_resv_to_send[i]) {
                wattrset(pWnd, COLOR_PAIR(4));
                sel_mode = 6;
            }
            else if(all_joint_servo_switch[i]) {
                wattrset(pWnd, COLOR_PAIR(8));
                sel_mode = 5;
            }
            else {
                wattrset(pWnd, COLOR_PAIR(1));
                sel_mode = 5;
            }
        }
        else if(all_joint_resv_to_send[i]){
            wattrset(pWnd, COLOR_PAIR(3));
            sel_mode = 3;
        }
        else if(all_joint_servo_switch[i]) {
            wattrset(pWnd, COLOR_PAIR(7));
            sel_mode = 7;
        }
        else {
            wattrset(pWnd, COLOR_PAIR(0));
            sel_mode = 0;
        }

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[0] + x_offset);
        waddstr(pWnd, joint_hydra_names[i]);
  
//        pthread_mutex_lock( &(thread_data->mutex) );

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[1] + x_offset);
        wprintw(pWnd, "%06.2f", RAD2DEG(hydraData->jnt.act.pos[i]));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
        wprintw(pWnd,"%06.2f", RAD2DEG(hydraData->jnt.ref_checked.pos[i]));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
        waddstr(pWnd, (hydraData->jnt.ref_checked.enabled[i] ? "ON" : "OFF"));
        //waddstr(pWnd, (all_joint_servo_switch[i] ? "ON" : "OFF"));

        /*wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[4] + x_offset);
        wprintw(pWnd,"%06.2f", hydraData->GetJointStatePtr(0)[i].DATA.tau_act);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[5] + x_offset);
        wprintw(pWnd,"%06.2f", all_joint_tau_fk[i]);
	*/
//        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[4] + x_offset);
//        wprintw(pWnd,"%05.1f", all_joint_freq_hz[i]);
	//ampl

//        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[5] + x_offset);
//        wprintw(pWnd,"%05.1f", all_joint_ampl[i]*180/M_PI);
        
        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[6] + x_offset);
        wprintw(pWnd,"%06.2f", RAD2DEG(all_joint_finalpos[i]));

//        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[7] + x_offset);
//        wprintw(pWnd,"%05.1f", hydraData->GetEHAStatePtr(0)[i].DATA.tau3_act);


//        pthread_mutex_unlock( &(thread_data->mutex) );

        wattrset(pWnd, COLOR_PAIR(sel_mode));
        switch(col_sel) {
        case 0:
            wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
            wprintw(pWnd, "%07.2f",RAD2DEG(hydraData->jnt.ref_checked.pos[i]));
            //wprintw(pWnd, "%07.2f",RAD2DEG(hydraData->GetJointCmdPtr(0)[i].DATA.pos_ref));
            break;
        case 1:
            wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
            waddstr(pWnd, (hydraData->jnt.ref_checked.enabled[i] ? "ON" : "OFF"));
            //waddstr(pWnd, (all_joint_servo_switch[i] ? "ON" : "OFF"));
            break;
        default:
            break;
        }

    }

    wattrset(pWnd, COLOR_PAIR(0));
}

void print_labels_joint(WINDOW *pLabelWnd)
{
    int x_offset = 1;

    wattron(pLabelWnd, A_BOLD);
    wmove(pLabelWnd, 0, label_x_pos[0]+x_offset);
    waddstr(pLabelWnd, "Joint Name");
 
    wmove(pLabelWnd, 0, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "Act Pos");

    wmove(pLabelWnd, 1, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "  [deg]");

    wmove(pLabelWnd, 0, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "Ref Pos");

    wmove(pLabelWnd, 1, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "  [deg]");

    wmove(pLabelWnd, 0, label_x_pos[3]+x_offset);
    waddstr(pLabelWnd, "Pwr");

    /*    wmove(pLabelWnd, 0, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "Act Tau");

    wmove(pLabelWnd, 1, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "   [Nm]");

    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "Grav Tau");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "  [Nm]");
    */
    wmove(pLabelWnd, 0, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "Freq");

    wmove(pLabelWnd, 1, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, " [Hz]");

    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "Ampl");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, " [deg]");


    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "Fin Pos");
    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "  [deg]");

    wmove(pLabelWnd, 0, label_x_pos[7]+x_offset);
    wprintw(pLabelWnd, "m_cur");

    wmove(pLabelWnd, 1, label_x_pos[7]+x_offset);
    wprintw(pLabelWnd, " [A]");


    wattroff(pLabelWnd, A_BOLD);
}

void print_menu_joint(WINDOW *pMenuWnd)
{
    int menu_width = 15;
    int x_offset = 1;
    wattrset(pMenuWnd, COLOR_PAIR(3));
    //if(hydraData->interp_run || hydraData->filemotion_run)
    if(hydraData->flags["interp_run"] || hydraData->flags["filemotion_run"])
        wattrset(pMenuWnd, COLOR_PAIR(8));
    wmove(pMenuWnd, 0, 0+x_offset);
    waddstr(pMenuWnd, "SPC:Tgl En");

    wmove(pMenuWnd, 0, menu_width+x_offset);
    waddstr(pMenuWnd, "F12:Send All");

    wmove(pMenuWnd, 0, menu_width*2+x_offset);
    waddstr(pMenuWnd, "d/D:Jog-");

    wmove(pMenuWnd, 0, menu_width*3+x_offset);
    waddstr(pMenuWnd, "DEL:All Off");

    wmove(pMenuWnd, 0, menu_width*4+x_offset);
    waddstr(pMenuWnd, "q:Quit");

    wattrset(pMenuWnd, COLOR_PAIR(0));
}

void print_title_joint(WINDOW *pTitleWnd)
{
    char str[256];
    int size_x;
    int size_y;
    werase(pTitleWnd);
    wmove(pTitleWnd, 0, 2);
    waddstr(pTitleWnd, "[Joint Mode]  Hydra Joint Control");

    wmove(pTitleWnd, 1, 2);

    //if(thread_data->interp_ready) {
    if(hydraData->flags["interp_ready"]) {
//        sprintf(str, " [R]");
        waddstr(pTitleWnd, "[R]  ");
    } else {
        waddstr(pTitleWnd, "[ ]  ");
    }
    waddstr(pTitleWnd, motion_name);

    getmaxyx(pTitleWnd, size_y, size_x);

//    if(thread_data->interp_run) {
    if(hydraData->flags["interp_ready"]) {
        sprintf(str, "     [Interpolating %5d/%5d]", hydraData->interp_cnt, hydraData->interp_length);
        wmove(pTitleWnd, 1, size_x-strlen(str)-2);
        waddstr(pTitleWnd, str);
//    } else if(thread_data->filemotion_run) {
    } else if(hydraData->flags["filemotion_run"]) {
        sprintf(str, "     [Motion Running %5d/%5d]", motion_line, motion_length);
        wmove(pTitleWnd, 1, size_x-strlen(str)-2);
        waddstr(pTitleWnd, str);
    }
}

void print_row_EHA(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end)
{
    int i;
    int height =1;
    int x_offset = 1;
    int y_offset = 0;
    //int sel_mode=0;
    wbkgd(pWnd, COLOR_PAIR(0));
    for( i=jnt_start; i<jnt_end; i++ ) {
        if(i==jnt_sel) {
            //if( hydraData->GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i] ) {
            if( hydraData->eha.ref.status[i]==EHA_CtrlWd_ON[i] ) {
                wattrset(pWnd, COLOR_PAIR(8));
//                sel_mode = 5;
            }
            else {
                wattrset(pWnd, COLOR_PAIR(1));
//                sel_mode = 5;
            }
        }
        //else if(hydraData->GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i]) {
        else if( hydraData->eha.ref.status[i]==EHA_CtrlWd_ON[i] ) {
            wattrset(pWnd, COLOR_PAIR(7));
//            sel_mode = 7;
        }
        else {
            wattrset(pWnd, COLOR_PAIR(0));
//            sel_mode = 0;
        }

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[0] + x_offset);
        waddstr(pWnd, eha_names[i]);

//        pthread_mutex_lock( &(thread_data->mutex) );

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[1] + x_offset);
        if(debug_mode)
        {
            //wprintw(pWnd, "%08x", hydraData->GetEHAStatePtr(0)[i].DATA.rawpos_act);
            wprintw(pWnd, "%08x", hydraData->eha.act.enc_raw[i]);
        }else{
            //wprintw(pWnd, "%07.2f", (hydraData->GetEHAStatePtr(0)[i].DATA.pos_act)*1000);
            wprintw(pWnd, "%07.2f", (hydraData->eha.act.pos[i])*1000);
        }
//        wprintw(pWnd, "%08x", all_eha_rawpos[i] );

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);

        if(debug_mode)
        {
            //wprintw(pWnd, "%08x", hydraData->GetEHACmdPtr(0)[i].DATA.rawpos_ref);
            wprintw(pWnd, "%08x", hydraData->eha.ref.enc_raw[i]);
        }else{
            //wprintw(pWnd, "%07.2f", (hydraData->GetEHACmdPtr(0)[i].DATA.pos_ref)*1000);
            wprintw(pWnd, "%07.2f", (hydraData->eha.ref.pos[i])*1000);
        }

//  wprintw(pWnd,"%07.2f", RAD2DEG(all_eha_refpos[i]));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
        waddstr(pWnd, ( (hydraData->eha.ref.status[i]==EHA_CtrlWd_ON[i]) ? "ON" : "OFF"));
        //waddstr(pWnd, ( (hydraData->GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i]) ? "ON" : "OFF"));
        //waddstr(pWnd, ( (all_eha_servo_switch[i]==EHA_CtrlWd_ON[i]) ? "ON" : "OFF"));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[4] + x_offset);
        wprintw(pWnd,"%08.2f", hydraData->eha.act.tau[i]);
        //wprintw(pWnd,"%08.2f", hydraData->GetEHAStatePtr(0)[i].DATA.tau_act);//Tau

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[5] + x_offset);
        wprintw(pWnd,"%08.2f", hydraData->eha.act.tau2[i]);
        //wprintw(pWnd,"%07.2f", hydraData->GetEHAStatePtr(0)[i].DATA.tau2_act);//ref_cur

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[6] + x_offset);
        wprintw(pWnd,"%08.2f", hydraData->eha.act.tau3[i]);
        //wprintw(pWnd,"%07.2f", hydraData->GetEHAStatePtr(0)[i].DATA.tau3_act);//msr_cur

//        pthread_mutex_unlock( &(thread_data->mutex) );
    }

    wattrset(pWnd, COLOR_PAIR(0));
}

void print_labels_EHA(WINDOW *pLabelWnd)
{
    int x_offset = 1;

    wattron(pLabelWnd, A_BOLD);

    wmove(pLabelWnd, 0, label_x_pos[0]+x_offset);
    waddstr(pLabelWnd, "EHA name");
 
    wmove(pLabelWnd, 0, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "Act Pos");

    wmove(pLabelWnd, 1, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "[cnt]");

    wmove(pLabelWnd, 0, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "Ref Pos");

    wmove(pLabelWnd, 1, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "[mm/deg]");

    wmove(pLabelWnd, 0, label_x_pos[3]+x_offset);
    waddstr(pLabelWnd, "State");

    wmove(pLabelWnd, 0, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "Tau");

    wmove(pLabelWnd, 1, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "[N/Nm]");

    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "ref_cur");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, " [A]");


    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "msr_cur");

    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, " [A]");

    wattroff(pLabelWnd, A_BOLD);
}

void print_menu_EHA(WINDOW *pMenuWnd)
{
    int menu_width = 15;
    int x_offset = 1;
    wattrset(pMenuWnd, COLOR_PAIR(3));

    wmove(pMenuWnd, 0, 0+x_offset);
    waddstr(pMenuWnd, "DEL:All Off");

    wmove(pMenuWnd, 0, menu_width+x_offset);
    waddstr(pMenuWnd, "q:Quit");

    wattrset(pMenuWnd, COLOR_PAIR(0));
}

void print_title_EHA(WINDOW *pTitleWnd)
{
    werase(pTitleWnd);
    wmove(pTitleWnd, 0, 2);
    waddstr(pTitleWnd, "[EHA Mode]    Hydra Joint Control");
}

void print_row_CART(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end)
{
    int i;
    int height =1;
    int x_offset = 1;
    int y_offset = 0;
    //int sel_mode=0;
    wbkgd(pWnd, COLOR_PAIR(0));

    for( i=jnt_start; i<jnt_end; i++ ) {

        if(i==jnt_sel) {
            wattrset(pWnd, COLOR_PAIR(1));
        }
            /*
            if( hydraData->GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i] ) {
                wattrset(pWnd, COLOR_PAIR(8));
//                sel_mode = 5;
            }
            else {
                wattrset(pWnd, COLOR_PAIR(1));
//                sel_mode = 5;
            }
        }
        else if(hydraData->GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i]) {
            wattrset(pWnd, COLOR_PAIR(7));
//            sel_mode = 7;
        }
        */
        else {
            wattrset(pWnd, COLOR_PAIR(0));
//            sel_mode = 0;
        }


        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[0] + x_offset);
//        waddstr(pWnd, eef_names[i]);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[1] + x_offset);
//        wprintw(pWnd,"%07.2f",eef_to_print[i]*1000);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
//        wprintw(pWnd,"%07.2f",eef_to_update[i]*1000);
    }

    wattrset(pWnd, COLOR_PAIR(0));
}


void print_labels_CART(WINDOW *pLabelWnd)
{
    int x_offset = 1;

    wattron(pLabelWnd, A_BOLD);

    wmove(pLabelWnd, 0, label_x_pos[0]+x_offset);
    waddstr(pLabelWnd, "EEF Name");

    wmove(pLabelWnd, 0, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "Pos");

    wmove(pLabelWnd, 1, label_x_pos[1]+x_offset);
    waddstr(pLabelWnd, "[mm/deg]");

    wmove(pLabelWnd, 0, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "Ref Pos");

    wmove(pLabelWnd, 1, label_x_pos[2]+x_offset);
    waddstr(pLabelWnd, "[mm/deg]");

    wmove(pLabelWnd, 0, label_x_pos[3]+x_offset);
    waddstr(pLabelWnd, "State"); //IK on/off

    wmove(pLabelWnd, 0, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "Tau");

    wmove(pLabelWnd, 1, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "[N/Nm]");

    /*
    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "ref_cur_");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, " [A]");


    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "msr_cur");

    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, " [A]");
    */

    wattroff(pLabelWnd, A_BOLD);
}

int show_quit_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y)
{
    int str=0;
    int win_size_x = 40;
    int win_size_y = 6;
    char msg[] = "Are you sure you want to quit?";
    WINDOW *pWndConfirm;
    pWndConfirm = subwin(pWnd, win_size_y, win_size_x, (scroll_panel_size_y>>1)-(win_size_y>>1), (size_x>>1)-(win_size_x>>1));
    werase(pWndConfirm);
    box(pWndConfirm, ACS_VLINE, ACS_HLINE);
    wbkgd(pWndConfirm, COLOR_PAIR(9));
    wmove(pWndConfirm, 1, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, msg);
    wmove(pWndConfirm, 2, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "All servo will turn off");
    wmove(pWndConfirm, 3, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "Press 'Y' to quit ");
    wmove(pWndConfirm, 4, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "Press 'N' to return ");
    wrefresh(pWndConfirm);

    while(str==0) {
        str = getch();
        switch(str) {
        case 'N':
        case 'n':
            return -1;
        case 'Y':
            return 0;
        default:
            str = 0;
            usleep(30000);
            break;
        }
    }
    return 0;
}

void show_help_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y)
{
    int str = 0;
    int win_size_x = 40;
    int win_size_y = 9;
    char msg[] = "Command List                         ";
    WINDOW *pWndConfirm;
    pWndConfirm = subwin(pWnd, win_size_y, win_size_x, (scroll_panel_size_y>>1)-(win_size_y>>1), (size_x>>1)-(win_size_x>>1));
    werase(pWndConfirm);
    box(pWndConfirm, ACS_VLINE, ACS_HLINE);
    wbkgd(pWndConfirm, COLOR_PAIR(2));
    wmove(pWndConfirm, 1, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, msg);
    wmove(pWndConfirm, 2, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "{U,u}/{D,d} Jog up/down joint angle");
    wmove(pWndConfirm, 3, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "A/a Increase/Decrease Sin amplitude");
    wmove(pWndConfirm, 4, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "F/f Increase/Decrease Sin frequency");
    wmove(pWndConfirm, 5, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "x Interpolate to initial position");
    wmove(pWndConfirm, 6, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "r run motion");
    wmove(pWndConfirm, 7, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "Press Enter to return ");
    wrefresh(pWndConfirm);
    while(str==0) {
        str = getch();
        if(str!='\n')
            str = 0;
        usleep(30000);
    }
}
/*
void turn_off_all_EHA(void)  // this function does not work!!!
{
    int j, k, l;
    
    for(j=0; j<HYDRA_JNT_MAX; j++) {
        all_joint_servo_switch[j] = false;
        all_joint_resv_to_send[j] = false;
        for(l=0; l<joint_to_EHA_power[j][0]; l++) {
            k = joint_to_EHA_power[j][l+1];
//            pthread_mutex_lock( pmutex );
            hydraData->GetEHACmdPtr(1)[k].DATA.ctlword
                    = (all_joint_servo_switch[j]==true)?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
//            pthread_mutex_unlock( pmutex );
        }
    }
}
*/
/*
void turn_off_all_joint(void)  // this function does not work!!!
{
    for(int j=0; j<HYDRA_JNT_MAX; j++) {
        all_joint_servo_switch[j] = false;
        all_joint_resv_to_send[j] = false;
        hydraData->GetJointCmdPtr(1)[j].DATA.enable = 0;

    }
}
*/

int reload_motion_file(char *filename)
{
    FILE *fp_motion;
    int ret;
    int num_joints = 31;
    int      motion_length_tmp;
    double **motion_data_tmp;

    if( (fp_motion = fopen(filename,"r"))==NULL ) {
        fprintf(fp_log, "Cannot open motion file: %s\n", filename);
        fflush(fp_log);
        return -1;
    }

    motion_length_tmp = CheckMotionFileLength(fp_motion);

    fprintf(fp_log, "motion length =%d\n", motion_length_tmp);
    fflush(fp_log);

    motion_data_tmp = AllocateMotionData(num_joints, motion_length_tmp);
    if(motion_data_tmp==NULL) {
        fprintf(stderr, "Failed to allocate motion data\n");
        return -1;
    }

    ret = LoadMotionFile(fp_motion, motion_data_tmp, num_joints, motion_length_tmp);
    if(ret != motion_length_tmp) {
        fprintf(fp_log, 
                "motion length mismatch defined=%d actual=%d\n", 
                motion_length_tmp, ret);
        return -1;
    }
    fclose(fp_motion);
    fprintf(fp_log, "motion length = %d\n", motion_length_tmp);

    // free old motion
    FreeMotionData(motion_data, motion_length);

    motion_data   = motion_data_tmp;
    motion_length = motion_length_tmp;

    memcpy(all_joint_finalpos, &(motion_data[0][1]), sizeof(double)*num_joints);
    return 0;
}

int  show_dialog_get_filename(WINDOW *pWnd, char *filename, int size_x, int scroll_panel_size_y)
{
    int str=0;
    int win_size_x = 40;
    int win_size_y = 6;
    char msg[] = "Load motion file           ";
    WINDOW *pWndConfirm;
    char str_file[256];
    int str_len = 0;

    memset(str_file, 0, 256*sizeof(char));

    pWndConfirm = subwin(pWnd, win_size_y, win_size_x, (scroll_panel_size_y>>1)-(win_size_y>>1), (size_x>>1)-(win_size_x>>1));
    werase(pWndConfirm);
    box(pWndConfirm, ACS_VLINE, ACS_HLINE);
    wbkgd(pWndConfirm, COLOR_PAIR(2));
    wmove(pWndConfirm, 1, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, msg);
    wmove(pWndConfirm, 2, (win_size_x>>1)-(strlen(msg)>>1));
    //waddstr(pWndConfirm, str_file);
    for(int i=0; i<25-str_len; i++) {
        waddstr(pWndConfirm, "_");
    }
    wmove(pWndConfirm, 3, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "Press enter key to enter ");
    wmove(pWndConfirm, 4, (win_size_x>>1)-(strlen(msg)>>1));
    waddstr(pWndConfirm, "Press escape key to cancel ");
    wrefresh(pWndConfirm);

    while(str==0) {
        str = getch();
        switch(str) {
        case '\n':
            strncpy(filename, str_file, str_len);
            fprintf(fp_log, "%d\n", str_len);
            fflush(fp_log);
            break;
        case 0x1b:
            return -1;
        default:
            if( (str_len!=0)&&(str==0x0107) ) { //backspace
                str_len--;
                str_file[str_len] = '\0';
                wmove(pWndConfirm, 2, (win_size_x>>1)-(strlen(msg)>>1));
                if(str_len>25) {
                    waddstr(pWndConfirm, &(str_file[str_len-25]));
                } else {
                    waddstr(pWndConfirm, str_file);
                    for(int i=0; i<25-str_len; i++) {
                        waddstr(pWndConfirm, "_");
                    }
                }
                wrefresh(pWndConfirm);
            } else if( (str>32)&&(str<127) ) {
                str_len++;
                str_file[str_len-1] = str;
                wmove(pWndConfirm, 2, (win_size_x>>1)-(strlen(msg)>>1));
                if(str_len>25) {
                    waddstr(pWndConfirm, &(str_file[str_len-25]));
                } else {
                    waddstr(pWndConfirm, str_file);
                    for(int i=0; i<25-str_len; i++) {
                        waddstr(pWndConfirm, "_");
                    }
                }
                wrefresh(pWndConfirm);
            } else {
                usleep(30000);
            }
            str = 0;
            break;
        }
    }
    return 0;
}
