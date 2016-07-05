#include <ncurses.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "user_interface.h"
#include "servo_ui.h"
#include "hydra.h"
#include "robot_hydra_id.h"
#include "chydradatamanager.h"

//#define EHA_ON 0x0101
//#define EHA_OFF 0x0100

extern CHydraDataManager hydra_data;

//extern joint_state_t  jnt_state[HYDRA_JNT_MAX];
//extern joint_cmd_t    jnt_cmd[HYDRA_JNT_MAX];
//extern eha_state_t    eha_state[EHA_MAX];
//extern eha_cmd_t      eha_cmd[EHA_MAX];
extern double all_joint_tau_fk[HYDRA_JNT_MAX];
extern double all_joint_pos_tgt[HYDRA_JNT_MAX];

extern double all_joint_initpos[HYDRA_JNT_MAX];
extern double all_joint_finalpos[HYDRA_JNT_MAX];

extern double all_joint_ampl[HYDRA_JNT_MAX];
extern double all_joint_freq_hz[HYDRA_JNT_MAX];
extern double all_joint_phase[HYDRA_JNT_MAX];

extern bool   all_joint_servo_switch[HYDRA_JNT_MAX];
//extern unsigned short all_eha_switch[EHA_MAX];//
//extern double all_eha_pos[EHA_MAX];
//extern unsigned int all_eha_rawpos[EHA_MAX];
//extern double all_eha_vel[EHA_MAX];
//extern double all_eha_tau[EHA_MAX];
//extern double all_eha_tau2[EHA_MAX];
//extern double all_eha_tau3[EHA_MAX];

extern int thread_end_flg;

extern bool interp_run;
extern bool grasp_run;
extern int  interp_cnt;
extern int  interp_length;
extern bool interp_ready;
extern bool filemotion_run;
extern bool debug_mode;

extern double **motion_data;
extern int      motion_length;
extern int      motion_line;
extern char     motion_name[256];

extern FILE* fp_log;

static double all_joint_refpos_to_send[HYDRA_JNT_MAX];
bool all_joint_resv_to_send[HYDRA_JNT_MAX];

static thread_data_t *thread_data;

static void init_curses(void);
static void print_row_joint(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end);
static void print_labels_joint(WINDOW *pLabelWnd);
static void print_menu_joint(WINDOW *pMenuWnd);
static void print_title_joint(WINDOW *pTitleWnd);
static void print_row_EHA(WINDOW *pWnd, int jnt_sel, int col_sel, int jnt_start, int jnt_end);
static void print_labels_EHA(WINDOW *pLabelWnd);
static void print_menu_EHA(WINDOW *pMenuWnd);
static void print_title_EHA(WINDOW *pTitleWnd);
static int  show_quit_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y);
static void show_help_dialog(WINDOW *pWnd, int size_x, int scroll_panel_size_y);
static void turn_off_all_EHA(void);
static int  reload_motion_file(char *filename);
static int  show_dialog_get_filename(WINDOW *pWnd, char *filename, int size_x, int scroll_panel_size_y);

const int label_x_pos[]={0, 23, 32, 41, 46,56,66,76}; //52, 57, 66, 80};

enum E_VIEW_MODE {
    E_VIEW_JOINT,
    E_VIEW_EHA};

void *servo_ui(void *param)
{  
    WINDOW *pWnd;
    WINDOW *pLabelWnd;
    WINDOW *pMenuWnd;
    WINDOW *pTitleWnd;

    int view_mode=0;
    int x=0,y=0;
    int key;

    int size_x, size_y;
    int scroll_panel_size_y;
    int scroll_panel_size_x;
    int jnt_start;
    int jnt_end;
    int loop;

    thread_data = (thread_data_t*)param;

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

    if(view_mode==E_VIEW_JOINT)
        jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
    else if(view_mode==E_VIEW_EHA)
        jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;

    x = 1;

    //表示ループ  
    while(1){

        erase();

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

        refresh();//表示更新

//---------------------------------入力キーの処理-------------------------------------------
        key = getch();
    
        //矢印キーの処理
        switch(key){
        case '\t':
            view_mode++;
            view_mode &= 0x1;
            y = 0;
            jnt_start = 0;
            if(view_mode==E_VIEW_JOINT)
                jnt_end   = (scroll_panel_size_y>HYDRA_JNT_MAX)?HYDRA_JNT_MAX:scroll_panel_size_y;
            else if(view_mode==E_VIEW_EHA)
                jnt_end   = (scroll_panel_size_y>EHA_MAX)?EHA_MAX:scroll_panel_size_y;
            break;

        case KEY_UP:
            if(y==jnt_start) {
                if(jnt_start>0) {
                    jnt_start--;
                    jnt_end = jnt_start + scroll_panel_size_y;
                    if(view_mode==E_VIEW_JOINT)
                        jnt_end = (jnt_end>HYDRA_JNT_MAX-1)?HYDRA_JNT_MAX-1:jnt_end;
                    else if(view_mode==E_VIEW_EHA)
                        jnt_end = (jnt_end>EHA_MAX-1)?EHA_MAX-1:jnt_end;
                }
                y = jnt_start;
            }
            else
                y--;
            break;

        case KEY_DOWN:
            if(y==jnt_end-1) {
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
                y = jnt_end-1;
            }
            else
                y++;
            break;

        case KEY_RIGHT:
            // x++;
            // x &= 0x1;
            break;
        case KEY_LEFT :
            // x--;
            // x &= 0x1;
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
                    if(interp_run==true)
                        goto endo;
                }

                new_state = !all_joint_servo_switch[y];
                for(j=0; j<joint_to_joint_power[y][0]; j++) {
                    k = joint_to_joint_power[y][j+1];
//                    pthread_mutex_lock( &(thread_data->mutex) );
                    hydra_data.GetJointCmdPtr(1)[k].DATA.pos_ref
                            = hydra_data.GetJointStatePtr(0)[k].DATA.pos_act;
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
            if( (interp_run!=false)||(filemotion_run!=false) )
                goto endu;
            interp_ready = false;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        all_joint_refpos_to_send[y]
                                = hydra_data.GetJointCmdPtr(0)[y].DATA.pos_ref + DEG2RAD(0.01);
//                        pthread_mutex_lock( &(thread_data->mutex) );
//                        hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
                        all_joint_pos_tgt[y]
                            = CHK_LIM(all_joint_refpos_to_send[y],
                                      all_joint_angle_limit[y][0],all_joint_angle_limit[y][1]);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
        endu:
            break;

        case 'U':
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            interp_ready = false;
            if(view_mode==E_VIEW_JOINT) {
//                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        all_joint_refpos_to_send[y]
                                = hydra_data.GetJointCmdPtr(0)[y].DATA.pos_ref + DEG2RAD(0.1);
//                        pthread_mutex_lock( &(thread_data->mutex) );
//                        hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
                        all_joint_pos_tgt[y]
                            = CHK_LIM(all_joint_refpos_to_send[y],
                                      all_joint_angle_limit[y][0],all_joint_angle_limit[y][1]);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
//                }
            }
            break;

        case 'd':
        case '-':
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            interp_ready = false;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        all_joint_refpos_to_send[y]
                                = hydra_data.GetJointCmdPtr(0)[y].DATA.pos_ref - DEG2RAD(0.01);
//                        pthread_mutex_lock( &(thread_data->mutex) );
//                        hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
                        all_joint_pos_tgt[y]
                            = CHK_LIM(all_joint_refpos_to_send[y],
                                      all_joint_angle_limit[y][0],all_joint_angle_limit[y][1]);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;

        case 'D':
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            interp_ready = false;
            if(view_mode==E_VIEW_JOINT) {
                //               if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
                        all_joint_refpos_to_send[y]
                                = hydra_data.GetJointCmdPtr(0)[y].DATA.pos_ref -DEG2RAD(0.1);
//                        pthread_mutex_lock( &(thread_data->mutex) );
//                        hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
                        all_joint_pos_tgt[y]
                            = CHK_LIM(all_joint_refpos_to_send[y],
                                      all_joint_angle_limit[y][0],all_joint_angle_limit[y][1]);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                    //}
            }
            break;

        case 'A':
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        all_joint_ampl[y] = all_joint_ampl[y] + DEG2RAD(0.1);
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;
        case 'a':
            if( (interp_run!=false)||(filemotion_run!=false) )
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
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        all_joint_freq_hz[y] = all_joint_freq_hz[y] + 0.01;
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;
        case 'f':
            if( (interp_run!=false)||(filemotion_run!=false) )
                break;
            if(view_mode==E_VIEW_JOINT) {
                if(all_joint_servo_switch[y]) {
                    if(!all_joint_resv_to_send[y]) {
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        all_joint_freq_hz[y] = (all_joint_freq_hz[y]>0.01) ? (all_joint_freq_hz[y]-0.01) : all_joint_freq_hz[y];
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                }
            }
            break;

        case ' ':
            if(view_mode==E_VIEW_JOINT) {
                if(x==1){
                    int j, k;
                    bool new_state;
                    if(all_joint_servo_switch[y]==false) {
                        if(interp_run==true)
                            goto endsp;
                    }

                    new_state = !all_joint_servo_switch[y];
                    for(j=0; j<joint_to_joint_power[y][0]; j++) {
                        k = joint_to_joint_power[y][j+1];
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        hydra_data.GetJointCmdPtr(1)[k].DATA.pos_ref
                                = hydra_data.GetJointStatePtr(0)[k].DATA.pos_act;
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                        all_joint_servo_switch[k] = new_state;
                        all_joint_resv_to_send[k] =  true;
                    }
                }
            }
        endsp:
            break;

        case '=':
            if(view_mode==E_VIEW_JOINT) {
//                pthread_mutex_lock( &(thread_data->mutex) );
                hydra_data.GetJointCmdPtr(1)[y].DATA.pos_ref
                        = hydra_data.GetJointStatePtr(0)[y].DATA.pos_act;
//                pthread_mutex_unlock( &(thread_data->mutex) );
                all_joint_resv_to_send[y] = true;
            }
            break;

        case 'x':
            //interp_run = (interp_run==1) ? 0 : 1;
            if(interp_run==true) {
                interp_run = false;
            } else {
                interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydra_data.GetJointStatePtr(0)[loop].DATA.pos_act;
                    //all_joint_finalpos[loop] = motion_data[motion_line][loop+1];
                }
                interp_run = true;
            }
            break;

        case 'g':
            //interp_run = (interp_run==1) ? 0 : 1;
            if(grasp_run==true) {
                grasp_run = false;
            } else {
                interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydra_data.GetJointCmdPtr(0)[loop].DATA.pos_ref;
                    all_joint_finalpos[loop] = hydra_data.GetJointCmdPtr(0)[loop].DATA.pos_ref;
                }
                all_joint_finalpos[JOINT_HYDRA_RHAND_THB_FE] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_IDX] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_MID] = 0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_RNG] = 0;

                all_joint_finalpos[JOINT_HYDRA_LHAND_THB_FE] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_IDX] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_MID] = 0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_RNG] = 0;

                interp_run = true;
            }
            break;

        case 'G':
            //interp_run = (interp_run==1) ? 0 : 1;
            if(grasp_run==true) {
                grasp_run = false;
            } else {
                interp_cnt = 0;
                for (loop=0; loop<HYDRA_JNT_MAX; loop++){
                    all_joint_initpos[loop]  = hydra_data.GetJointCmdPtr(0)[loop].DATA.pos_ref;
                    all_joint_finalpos[loop] = hydra_data.GetJointCmdPtr(0)[loop].DATA.pos_ref;
                }
                all_joint_finalpos[JOINT_HYDRA_RHAND_THB_FE] = 3 * M_PI / 3.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_IDX] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_MID] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_RHAND_RNG] = -3 * M_PI / 2.0;

                all_joint_finalpos[JOINT_HYDRA_LHAND_THB_FE] = 3 * M_PI / 3.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_IDX] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_MID] = -3 * M_PI / 2.0;
                all_joint_finalpos[JOINT_HYDRA_LHAND_RNG] = -3 * M_PI / 2.0;

                grasp_run = true;
            }
            break;

        case 'r':
            if( (interp_ready==true)&&(filemotion_run==false) ) {
                filemotion_run = true;
            } else {
                filemotion_run = false;
            }
            break;
        case 'l':
        {
            char filename[256];
            memset(filename, 0, 256);
            if(show_dialog_get_filename(pWnd, filename, size_x, scroll_panel_size_y)==0) {
                fprintf(fp_log, "%s\n", filename);
                fflush(fp_log);
                reload_motion_file(filename);
                strcpy(motion_name, filename);
                interp_ready = false;
            }
            break;
        }
        case KEY_DC:
            turn_off_all_EHA( );
            break;

        case '?': // show help
            show_help_dialog(pWnd, size_x, scroll_panel_size_y);
            break;
        
        case 'q': //qで終了
        {
            if(show_quit_dialog(pWnd, size_x, scroll_panel_size_y)<0)
                goto skip_quit;

            turn_off_all_EHA( );

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
                if( (interp_run!=false)||(filemotion_run!=false) )
                    goto endcr;
                interp_ready = false;
                for(j=0; j<joint_to_joint_power[y][0]; j++) {
                    k = joint_to_joint_power[y][j+1];
                    hydra_data.GetJointCmdPtr(1)[k].DATA.pos_ref
                            = hydra_data.GetJointStatePtr(0)[k].DATA.pos_act;
                    //all_joint_servo_switch[k] = all_joint_servo_switch[k];
                    //hydra_data.GetJointCmdPtr(1)[k].DATA.enable = (all_joint_servo_switch[k]==true)?1:0;
                    all_joint_resv_to_send[k] = false;
                }
                /*
                for(j=0; j<joint_to_EHA_power[y][0]; j++) {
                    k = joint_to_EHA_power[y][j+1];
//                    pthread_mutex_lock( &(thread_data->mutex) );
                    hydra_data.GetEHACmdPtr(1)[k].DATA.ctlword
                            = (all_joint_servo_switch[y]==true)?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
//                    pthread_mutex_unlock( &(thread_data->mutex) );
                }
                */
            }
        endcr:
            break;

        case KEY_F(12):

            if(view_mode==E_VIEW_JOINT) {
                int j, k, l;

                if( (interp_run!=false)||(filemotion_run!=false) )
                    goto endf12;

                interp_ready = false;

                for(j=0; j<HYDRA_JNT_MAX; j++) {
                    hydra_data.GetJointCmdPtr(1)[j].DATA.pos_ref
                            = hydra_data.GetJointStatePtr(0)[j].DATA.pos_act;
                    all_joint_resv_to_send[j] = false;
                    /*
                    for(l=0; l<joint_to_EHA_power[j][0]; l++) {
                        k = joint_to_EHA_power[j][l+1];
//                        pthread_mutex_lock( &(thread_data->mutex) );
                        hydra_data.GetEHACmdPtr(1)[k].DATA.ctlword
                                = (all_joint_servo_switch[j]==true)?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
//                        pthread_mutex_unlock( &(thread_data->mutex) );
                    }
                    */
                }
            }
        endf12:
            break;

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
                        hydra_data.GetEHAStatePtr(0)[loop].DATA.rawpos_act);
            }
            fclose(fp);
            break;
        }

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
        wprintw(pWnd, "%07.2f", RAD2DEG(hydra_data.GetJointStatePtr(0)[i].DATA.pos_act));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
        wprintw(pWnd,"%07.2f", RAD2DEG(hydra_data.GetJointCmdPtr(0)[i].DATA.pos_ref));



        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
        waddstr(pWnd, (all_joint_servo_switch[i] ? "ON" : "OFF"));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[4] + x_offset);
        wprintw(pWnd,"%07.2f", hydra_data.GetJointStatePtr(0)[i].DATA.tau_act);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[5] + x_offset);
        wprintw(pWnd,"%07.2f", all_joint_tau_fk[i]);

//        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[6] + x_offset);
//        wprintw(pWnd,"%07.2f", all_joint_freq_hz[i]);

        
        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[6] + x_offset);
        wprintw(pWnd,"%07.2f", RAD2DEG(all_joint_finalpos[i]));

//        pthread_mutex_unlock( &(thread_data->mutex) );

        wattrset(pWnd, COLOR_PAIR(sel_mode));
        switch(col_sel) {
        case 0:
            wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
            wprintw(pWnd, "%07.2f",RAD2DEG(hydra_data.GetJointCmdPtr(0)[i].DATA.pos_ref));
            break;
        case 1:
            wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
            waddstr(pWnd, (all_joint_servo_switch[i] ? "ON" : "OFF"));
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

    wmove(pLabelWnd, 0, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "Act Tau");

    wmove(pLabelWnd, 1, label_x_pos[4]+x_offset);
    wprintw(pLabelWnd, "   [Nm]");

    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "Grav Tau");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "  [Nm]");

//    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
//    wprintw(pLabelWnd, "Freq");

//    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
//    wprintw(pLabelWnd, "   [Hz]");

    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "Final Pos");
    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "  [deg]");



    wattroff(pLabelWnd, A_BOLD);
}

void print_menu_joint(WINDOW *pMenuWnd)
{
    int menu_width = 15;
    int x_offset = 1;
    wattrset(pMenuWnd, COLOR_PAIR(3));
    if(interp_run || filemotion_run ) wattrset(pMenuWnd, COLOR_PAIR(8));
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

    if(interp_ready) {
//        sprintf(str, " [R]");
        waddstr(pTitleWnd, "[R]  ");
    } else {
        waddstr(pTitleWnd, "[ ]  ");
    }
    waddstr(pTitleWnd, motion_name);

    getmaxyx(pTitleWnd, size_y, size_x);

    if(interp_run) {
        sprintf(str, "     [Interpolating %5d/%5d]", interp_cnt, interp_length);
        wmove(pTitleWnd, 1, size_x-strlen(str)-2);
        waddstr(pTitleWnd, str);
    } else if(filemotion_run) {
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
            if( hydra_data.GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i] ) {
                wattrset(pWnd, COLOR_PAIR(8));
//                sel_mode = 5;
            }
            else {
                wattrset(pWnd, COLOR_PAIR(1));
//                sel_mode = 5;
            }
        }
        else if(hydra_data.GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i]) {
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
            wprintw(pWnd, "%08x", hydra_data.GetEHAStatePtr(0)[i].DATA.rawpos_act);
        }else{
            wprintw(pWnd, "%07.2f", (hydra_data.GetEHAStatePtr(0)[i].DATA.pos_act)*1000);
        }
//        wprintw(pWnd, "%08x", all_eha_rawpos[i] );

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[2] + x_offset);
        /*
        if(debug_mode)
        {
            wprintw(pWnd, "%08x", hydra_data.GetEHACmdPtr(0)[i].DATA.rawpos_ref);
        }else{
            wprintw(pWnd, "%07.2f", (hydra_data.GetEHACmdPtr(0)[i].DATA.pos_ref)*1000);
        }
        */
//  wprintw(pWnd,"%07.2f", RAD2DEG(all_eha_refpos[i]));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[3] + x_offset);
        waddstr(pWnd, ( (hydra_data.GetEHACmdPtr(0)[i].DATA.ctlword==EHA_CtrlWd_ON[i]) ? "ON" : "OFF"));
        //waddstr(pWnd, ( (all_eha_servo_switch[i]==EHA_CtrlWd_ON[i]) ? "ON" : "OFF"));

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[4] + x_offset);
        wprintw(pWnd,"%07.2f", hydra_data.GetEHAStatePtr(0)[i].DATA.tau_act);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[5] + x_offset);
        wprintw(pWnd,"%07.2f", hydra_data.GetEHAStatePtr(0)[i].DATA.tau2_act);

        wmove(pWnd, height*(i-jnt_start) + y_offset, label_x_pos[6] + x_offset);
        wprintw(pWnd,"%07.2f", hydra_data.GetEHAStatePtr(0)[i].DATA.tau3_act);

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
    wprintw(pLabelWnd, "Act Tau");

    wmove(pLabelWnd, 0, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "Strain");

    wmove(pLabelWnd, 1, label_x_pos[5]+x_offset);
    wprintw(pLabelWnd, "[bit]");


    wmove(pLabelWnd, 0, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "Temp");

    wmove(pLabelWnd, 1, label_x_pos[6]+x_offset);
    wprintw(pLabelWnd, "C");

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

void turn_off_all_EHA(void)
{
    int j, k, l;
    
    for(j=0; j<HYDRA_JNT_MAX; j++) {
        all_joint_servo_switch[j] = false;
        all_joint_resv_to_send[j] = false;
        for(l=0; l<joint_to_EHA_power[j][0]; l++) {
            k = joint_to_EHA_power[j][l+1];
//            pthread_mutex_lock( pmutex );
            hydra_data.GetEHACmdPtr(1)[k].DATA.ctlword
                    = (all_joint_servo_switch[j]==true)?EHA_CtrlWd_ON[k]:EHA_CtrlWd_OFF[k];
//            pthread_mutex_unlock( pmutex );
        }
    }
}

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
