/*-----------------------------------------------------------------------------
 * SlaveInfo.h
 * Copyright                acontis technologies GmbH, Weingarten, Germany
 * Response                 Stefan Zintgraf
 * Description              Slave Info header
 *---------------------------------------------------------------------------*/

#if !(defined __SLAVEINFO_H__)
#define __SLAVEINFO_H__     1

/*-INCLUDES------------------------------------------------------------------*/

/*-MACROS--------------------------------------------------------------------*/


/*-GLOBAL VARIABLES-----------------------------------------------------------*/


/*-TYPEDEFS------------------------------------------------------------------*/
typedef enum _T_eEtherCAT_Vendor
{
    ecvendor_etg                                = 0x00000001,
    ecvendor_beckhoff                           = 0x00000002,
    ecvendor_scuola_superiore_s_anna            = 0x00000003,
    ecvendor_ixxat                              = 0x00000004,
    ecvendor_vector_informatik                  = 0x00000005,
    ecvendor_knestel                            = 0x00000006,
    ecvendor_cmz_sistemi                        = 0x0000000a,
    ecvendor_softing                            = 0x0000000d,
    ecvendor_microcontrol                       = 0x0000000e,
    ecvendor_pollmeier                          = 0x0000000f,
    ecvendor_lust                               = 0x00000010,
    ecvendor_kuebler                            = 0x00000013,
    ecvendor_keb                                = 0x00000014,
    ecvendor_lti                                = 0x00000016,
    ecvendor_esd_electronic_design              = 0x00000017,
    ecvendor_hms_industrial_networks            = 0x0000001b,
    ecvendor_epis_automation                    = 0x0000001c,
    ecvendor_festo                              = 0x0000001D,
    ecvendor_wago                               = 0x00000021,
    ecvendor_boschrexroth                       = 0x00000024,
    ecvendor_moog                               = 0x00000028,
    ecvendor_port                               = 0x00000034,
    ecvendor_buerkert_werke                     = 0x00000039,
    ecvendor_lenze                              = 0x0000003B,
    ecvendor_tigris_electronic                  = 0x00000042,
    ecvendor_hilscher                           = 0x00000044,
    ecvendor_murrelektronik                     = 0x0000004F,
    ecvendor_bombardier_transportation          = 0x00000051,
    ecvendor_komax                              = 0x00000057,
    ecvendor_sew_eurodrive                      = 0x00000059,
    ecvendor_bachmann_electronic                = 0x00000062,
    ecvendor_danaher                            = 0x0000006A,
    ecvendor_kollmorgen                         = 0xE000006A,
    ecvendor_woodward_seg                       = 0x0000006b,
    ecvendor_bernecker_rainer_ie                = 0x0000006c,
    ecvendor_infranor_electronics               = 0x00000082,
    ecvendor_omron                              = 0x00000083,
    ecvendor_gefran                             = 0x00000093,
    ecvendor_elmo_motion                        = 0x0000009a,
    ecvendor_sontheim_industrie_elektronik      = 0x000000a0,
    ecvendor_hirschmann_automation              = 0x000000a5,
    ecvendor_copley                             = 0x000000AB,
    ecvendor_pepperl_fuchs                      = 0x000000ad,
    ecvendor_johannes_huebner                   = 0x000000af,
    ecvendor_abb_oy_drives                      = 0x000000b7,
    ecvendor_stoeber                            = 0x000000b9,
    ecvendor_messung_systems                    = 0x000000ce,
    ecvendor_bonfiglioli_vectron                = 0x000000d5,
    ecvendor_phase_motion_control               = 0x000000d9,
    ecvendor_metronix                           = 0x000000e4,
    ecvendor_ascon                              = 0x000000e9,
    ecvendor_controltechniques                  = 0x000000F9,
    ecvendor_maxon_motor                        = 0x000000FB,
    ecvendor_keba                               = 0x00000105,
    ecvendor_wittenstein                        = 0x0000010a,
    ecvendor_twk_elektronik                     = 0x0000010d,
    ecvendor_psa_elettronica                    = 0x0000010f,
    ecvendor_smc                                = 0x00000114,
    ecvendor_jvl_industri_elektronik            = 0x00000117,
    ecvendor_hottinger_baldwin                  = 0x0000011d,
    ecvendor_leuze_electronic                   = 0x00000121,
    ecvendor_jumo                               = 0x00000126,
    ecvendor_hsd                                = 0x00000129,
    ecvendor_lika_electronic                    = 0x0000012e,
    ecvendor_csm                                = 0x0000012f,
    ecvendor_lpkf_motion_control                = 0x00000146,
    ecvendor_imc_messysteme                     = 0x0000014a,
    ecvendor_baumueller                         = 0x0000015A,
    ecvendor_pneumax                            = 0x0000017a,
    ecvendor_promess                            = 0x00000190,
    ecvendor_deutschmann                        = 0x0000019d,
    ecvendor_golden                             = 0x0000019e,
    ecvendor_brunner_elektronik                 = 0x000001a1,
    ecvendor_technosoft                         = 0x000001a3,
    ecvendor_peyer_engineering                  = 0x000001b4,
    ecvendor_robox                              = 0x000001B5,
    ecvendor_parker	                            = 0xE00001B5,
    ecvendor_sanyo_denki                        = 0x000001b9,
    ecvendor_delta_electronics                  = 0x000001dd,
    ecvendor_amk                                = 0x000001eb,
    ecvendor_national_instruments               = 0x000001F9,
    ecvendor_fernsteuergeraete_kurt_oelsch      = 0x000001fc,
    ecvendor_idam                               = 0x000001FD,
    ecvendor_prueftechnik_ndt                   = 0x000001fe,
    ecvendor_baumer_th                          = 0x00000204,
    ecvendor_esitron_electronic                 = 0x00000207,
    ecvendor_systeme_helmholz                   = 0x00000223,
    ecvendor_pantec                             = 0x00000225,
    ecvendor_abb_stotz_kontakt                  = 0x0000023a,
    ecvendor_berghof_automationstechnik         = 0x0000023b,
    ecvendor_stotz_feinmesstechnik              = 0x0000024f,
    ecvendor_dunkermotoren                      = 0x00000257,
    ecvendor_roche_diagnostics                  = 0x00000283,
    ecvendor_toshiba_schneider                  = 0x00000284,
    ecvendor_bihl_wiedemann                     = 0x00000285,
    ecvendor_trinamic_motion_control            = 0x00000286,
    ecvendor_performance_motion_devices         = 0x00000296,
    ecvendor_ingenia_cat                        = 0x0000029c,
    ecvendor_crevis                             = 0x0000029d,
    ecvendor_thk                                = 0x000002d0,
    ecvendor_digitronic                         = 0x00000302,
    ecvendor_hanyang                            = 0x000003ae,
    ecvendor_baldor_uk                          = 0x000003db,
    ecvendor_beck_ipc                           = 0x00000501,
    ecvendor_etas                               = 0x00000502,
    ecvendor_phytec_messtechnik                 = 0x00000504,
    ecvendor_anca_motion                        = 0x00000505,
    ecvendor_fh_koeln                           = 0x00000506,
    ecvendor_nuvation_research                  = 0x00000508,
    ecvendor_tr                                 = 0x00000509,
    ecvendor_gantner                            = 0x0000050a,
    ecvendor_mks_systems                        = 0x0000050b,
    ecvendor_abb_robotics                       = 0x0000050c,
    ecvendor_unitro_fleischmann                 = 0x0000050d,
    ecvendor_zub_machine_control                = 0x0000050e,
    ecvendor_dspace                             = 0x0000050f,
    ecvendor_samsung                            = 0x00000511,
    ecvendor_bce                                = 0x00000512,
    ecvendor_jaeger_messtechnik                 = 0x00000513,
    ecvendor_tetra                              = 0x00000514,
    ecvendor_justek                             = 0x00000515,
    ecvendor_baumer_thalheim                    = 0x00000516,
    ecvendor_elin_ebg_traction                  = 0x00000517,
    ecvendor_meka_robotics                      = 0x00000518,
    ecvendor_altera_japan                       = 0x00000519,
    ecvendor_ebv_elektronik                     = 0x0000051a,
    ecvendor_igh                                = 0x0000051b,
    ecvendor_iav                                = 0x0000051c,
    ecvendor_hitachi                            = 0x0000051d,
    ecvendor_tenasys                            = 0x0000051e,
    ecvendor_pondis                             = 0x0000051f,
    ecvendor_moog_italiana                      = 0x00000520,
    ecvendor_wallner_automation                 = 0x00000522,
    ecvendor_avl_list                           = 0x00000523,
    ecvendor_ritter_elektronik                  = 0x00000524,
    ecvendor_zwick                              = 0x00000527,
    ecvendor_dresdenelektronik                  = 0x00000528,
    ecvendor_philips_healthcare                 = 0x0000052c,
    ecvendor_chess                              = 0x0000052d,
    ecvendor_nct                                = 0x0000052e,
    ecvendor_anywire                            = 0x0000052f,
    ecvendor_shadow_robot                       = 0x00000530,
    ecvendor_fecon                              = 0x00000531,
    ecvendor_fh_suedwestfahlen                  = 0x00000532,
    ecvendor_add2                               = 0x00000533,
    ecvendor_arm_automation                     = 0x00000534,
    ecvendor_knapp_logistik                     = 0x00000537,
    ecvendor_getriebebau_nord                   = 0x00000538,
    ecvendor_yaskawa                            = 0x00000539,
    ecvendor_oki                                = 0x0000053a,
    ecvendor_takasaki_kyoudou                   = 0x0000053b,
    ecvendor_nittetsu_elex                      = 0x0000053c,
    ecvendor_unjo                               = 0x0000053e,
    ecvendor_eads_deutschland                   = 0x0000053f,
    ecvendor_acs_motion_control                 = 0x00000540,
    ecvendor_keyence                            = 0x00000541,
    ecvendor_mefi                               = 0x00000542,
    ecvendor_mut                                = 0x00000543,
    ecvendor_isw_uni_stuttgart                  = 0x00000544,
    ecvendor_elsena                             = 0x00000545,
    ecvendor_be_semiconductor                   = 0x00000546,
    ecvendor_hauni_lni                          = 0x00000547,
    ecvendor_etel                               = 0x00000548,
    ecvendor_vat_vakuumventile                  = 0x00000549,
    ecvendor_laytec                             = 0x0000054a,
    ecvendor_num                                = 0x0000054b,
    ecvendor_hauni_maschinenbau                 = 0x0000054c,
    ecvendor_exatronic                          = 0x0000054d,
    ecvendor_iim_chinese_aos                    = 0x0000054e,
    ecvendor_tu_eindhoven                       = 0x0000054f,
    ecvendor_scansonic                          = 0x00000550,
    ecvendor_shanghai_sodick_sw                 = 0x00000551,
    ecvendor_chuo_electronics                   = 0x00000552,
    ecvendor_agie                               = 0x00000553,
    ecvendor_hei_canton_de_vaud                 = 0x00000555,
    ecvendor_jenny_science                      = 0x00000557,
    ecvendor_industrial_control_communications  = 0x00000558,
    ecvendor_ckd_elektrotechnika                = 0x0000055a,
    ecvendor_qem                                = 0x0000055b,
    ecvendor_simatex                            = 0x0000055c,
    ecvendor_kithara                            = 0x0000055d,
    ecvendor_converteam                         = 0x0000055e,
    ecvendor_ara                                = 0x0000055f,
    ecvendor_tata_consultancy                   = 0x00000560,
    ecvendor_tiab                               = 0x00000562,
    ecvendor_rkc_instrument                     = 0x00000563,
    ecvendor_switched_reluctance                = 0x00000564,
    ecvendor_avnet_electronics                  = 0x00000566,
    ecvendor_abb_force_measurement              = 0x00000567,
    ecvendor_kunbus                             = 0x00000569,
    ecvendor_acd_antriebstechnik                = 0x0000056a,
    ecvendor_bronkhorst                         = 0x0000056b,
    ecvendor_k_mecs                             = 0x0000056c,
    ecvendor_thomson_broadcast                  = 0x0000056d,
    ecvendor_ufg_elettronica                    = 0x0000056e,
    ecvendor_xilinx                             = 0x0000056f,
    ecvendor_abb_power_systems                  = 0x00000570,
    ecvendor_servoland                          = 0x00000571,
    ecvendor_hivertec                           = 0x00000572,
    ecvendor_fike_europe                        = 0x00000573,
    ecvendor_ropex                              = 0x00000576,
    ecvendor_tlu                                = 0x00000577,
    ecvendor_prodrive                           = 0x00000579,
    ecvendor_miho_inspektionssysteme            = 0x0000057a,
    ecvendor_tokyo_electron                     = 0x0000057b,
    ecvendor_lintec                             = 0x0000057c,
    ecvendor_simplex_vision                     = 0x0000057d,
    ecvendor_sus                                = 0x00000581,
    ecvendor_trsystems                          = 0x00000582,
    ecvendor_harmonic_drive                     = 0x00000583,
    ecvendor_staeubli_faverges                  = 0x00000584,
    ecvendor_scienlab_electronic                = 0x00000585,
    ecvendor_fujisoft                           = 0x00000587,
    ecvendor_iai_corporation                    = 0x00000588,
    ecvendor_promavtomatika                     = 0x00000589,
    ecvendor_kistler_instrumente                = 0x0000058a,
    ecvendor_lauda_wobser                       = 0x0000058b,
    ecvendor_schweitzer_engineering_labs        = 0x0000058c,
    ecvendor_mutracx                            = 0x0000058e,
    ecvendor_algo                               = 0x0000058f,
    ecvendor_muehlbauer                         = 0x00000590,
    ecvendor_sealevel_systems                   = 0x00000592,
    ecvendor_igm_robotersysteme                 = 0x00000593,
    ecvendor_zbe                                = 0x00000595,
    ecvendor_schneider_electric                 = 0x00000596,
    ecvendor_fraunhofer_iosb_ina                = 0x00000597,
    ecvendor_skf_magnetic_bearings              = 0x00000598,
    ecevndor_galil_motion_control               = 0x00000599,
    ecvendor_ihi                                = 0x0000059a,
    ecvendor_wenglor_sensoric                   = 0x0000059b,
    ecvendor_ingeteam                           = 0x0000059c,
    ecvendor_micro_vu                           = 0x0000059e,
    ecvendor_oehri_electronic                   = 0x0000059f,
    ecvendor_nagano_oki                         = 0x00000600,
    ecvendor_condalo                            = 0x00000601,
    ecvendor_tg_drives                          = 0x00000666,
    ecvendor_schleuniger                        = 0x0000066e,
    ecvendor_koenig                             = 0x00000777,
    ecvendor_shanghai_cnc                       = 0x00000900,
    ecvendor_mitsubishi                         = 0x00000a1e,
    ecvendor_john_deere                         = 0x00000d05,
    ecvendor_cantops                            = 0x00001122,
    ecvendor_ids                                = 0x000012ad,
    ecvendor_adlink                             = 0x0000144a,
    ecvendor_eubus                              = 0x0000147a,
    ecvendor_unico                              = 0x00001502,
    ecvendor_dlr                                = 0x00001616,
    ecvendor_hei_canton_de_vaud_reds            = 0x0000179a,
    ecvendor_tamagawa_seiki			            = 0x00001800, //add
    ecvendor_bystorm                            = 0x00001a21,
    ecvendor_ipetronik                          = 0x00001a90,
    ecvendor_sennheiser                         = 0x00001b66,
    ecvendor_danieli_automation                 = 0x00002304,
    ecvendor_acontis                            = 0x00004154,
    ecvendor_kuka                               = 0x000060c8,
    ecvendor_nat                                = 0x00006c78,
    ecvendor_mecapion                           = 0x00007595,
    ecvendor_ontec                              = 0x00007604,
    ecvendor_foxnum                             = 0x00007715,
    ecvendor_kyoei                              = 0x00007716,
    ecvendor_brother                            = 0x00008562,
    ecvendor_shenyang_machine_tool              = 0x00008818,
    ecvendor_soft_servo                         = 0x00009555,
    ecvendor_vipa                               = 0x0000affe,
    ecvendor_gd                                 = 0x0000bebe,
    ecvendor_keba_at                            = 0x0000ceba,
    ecvendor_willow_garage                      = 0x0000eeee,
    ecvendor_interroll                          = 0x0000fe09,
    ecvendor_silica_avnet                       = 0x0000fedc,
    ecvendor_altima                             = 0x00414c54,
    ecvendor_kuhnke                             = 0x0048554B,
    ecvendor_jat                                = 0x004A4154,
    ecvendor_heidelberger                       = 0x004d4448,
    ecvendor_mecalc                             = 0x004d4543,
    ecvendor_samsung_sec                        = 0x00534543,
    ecvendor_aixcon                             = 0x00616978,
    ecvendor_kk_electronic                      = 0x00ae4b4b,
    ecvendor_sick                               = 0x01000056,
    ecvendor_parker_hannifin                    = 0x01000089,
    ecvendor_balluf                             = 0x010000e8,
    ecvendor_ma_vi                              = 0x01abcdef,
    ecvendor_kraeutner_software                 = 0x01c0ffee,
    ecvendor_parker_hannifin_eme                = 0x02000089,
    ecvendor_danfoss_drives                     = 0x0200008d,
    ecvendor_parker_hannifin_eme_630            = 0x03000089,
    ecvendor_parker_hannifin_ssd                = 0x04000089,
    ecvendor_schneider_motion_control           = 0x0800005a,
    ecvendor_fas                                = 0x0fa00000,
    ecvendor_beckhoff_hardware                  = 0x10000002,
    ecvendor_hengstler                          = 0x20041961,
    ecvendor_lenord_bauer                       = 0x20422b4c,
    ecvendor_ibv                                = 0x21494256,
    ecvendor_red_one                            = 0x40524f54,
    ecvendor_shf_communication                  = 0x46485320,
    ecvendor_grossenbacher                      = 0x47535953,
    ecvendor_nti_linmot                         = 0x4c4e5449,
    ecvendor_eltromat                           = 0x4c746c65,
    ecvendor_arte_motion                        = 0x65547241,
    ecvendor_paul_maschinenfabrik               = 0x7061756c,
    ecvendor_handtmann                          = 0xdeadbeef,
    
    /* Borland C++ datatype alignment correction */
    ecvendor_BCppDummy                          = 0xFFFFFFFF
} T_eEtherCAT_Vendor;

typedef enum _T_eEtherCAT_ProductCode
{
    /* Beckhoff Automation */                       
    ecprodcode_beck_AX2000_B110                 = 0x07d06014,
    ecprodcode_beck_AX2000_B120                 = 0x07d06014,
    ecprodcode_beck_BK1120                      = 0x04602c22,
    ecprodcode_beck_CX1100_0004                 = 0x044C6032,
    ecprodcode_beck_EK1100                      = 0x044c2c52,
    ecprodcode_beck_EK1101                      = 0x044d2c52,
    ecprodcode_beck_EK1122                      = 0x04622c52,
    ecprodcode_beck_EL1002                      = 0x03ea3052,
    ecprodcode_beck_EL1004                      = 0x03ec3052,
    ecprodcode_beck_EL1004_0010                 = 0x03EC3052,
    ecprodcode_beck_EL1012                      = 0x03f43052,
    ecprodcode_beck_EL1014                      = 0x03f63052,
    ecprodcode_beck_EL1014_0010                 = 0x03F63052,
    ecprodcode_beck_EL1018                      = 0x03fa3052,
    ecprodcode_beck_EL1094                      = 0x04463052,
    ecprodcode_beck_EL1904                      = 0x07703052,
    ecprodcode_beck_EL2002                      = 0x07d23052,
    ecprodcode_beck_EL2004                      = 0x07d43052,
    ecprodcode_beck_EL2008                      = 0x07d83052,
    ecprodcode_beck_EL2032                      = 0x07f03052,
    ecprodcode_beck_EL2502                      = 0x09c63052,
    ecprodcode_beck_EL2521                      = 0x09d93052,
    ecprodcode_beck_EL2521_1001                 = 0x09D93052,
    ecprodcode_beck_EL2904                      = 0x0B583052,
    ecprodcode_beck_EL3102                      = 0x0c1e3052,
    ecprodcode_beck_EL3112                      = 0x0c283052,
    ecprodcode_beck_EL3122                      = 0x0c323052,
    ecprodcode_beck_EL3142                      = 0x0c463052,
    ecprodcode_beck_EL3142_0010                 = 0x0c463052,
    ecprodcode_beck_EL3152                      = 0x0c503052,
    ecprodcode_beck_EL3162                      = 0x0c5a3052,
    ecprodcode_beck_EL3202                      = 0x0c823052,
    ecprodcode_beck_EL3312                      = 0x0cf03052,
    ecprodcode_beck_EL4002                      = 0x0fa23052,
    ecprodcode_beck_EL4004                      = 0x0fa43052,
    ecprodcode_beck_EL4102                      = 0x10063052,
    ecprodcode_beck_EL4112                      = 0x10103052,
    ecprodcode_beck_EL4112_0010                 = 0x10103052,
    ecprodcode_beck_EL4122                      = 0x101a3052,
    ecprodcode_beck_EL4132                      = 0x10243052,
    ecprodcode_beck_EL5001                      = 0x13893052,
    ecprodcode_beck_EL5101                      = 0x13ed3052,
    ecprodcode_beck_EL5151                      = 0x141f3052,
    ecprodcode_beck_AX5206                      = 0x14566012,  
    ecprodcode_beck_EL6001                      = 0x17713052,
    ecprodcode_beck_EL6021                      = 0x17853052,
    ecprodcode_beck_EL6601                      = 0x19C93052,
    ecprodcode_beck_EL6690                      = 0x1a223052,
    ecprodcode_beck_EL6692                      = 0x1a243052,
    ecprodcode_beck_EL6731                      = 0x1a4b3052,
    ecprodcode_beck_EL6731_0010                 = 0x1a4b3052,
    ecprodcode_beck_EL6751                      = 0x1a5f3052,
    ecprodcode_beck_EL6752                      = 0x1a603052,
    ecprodcode_beck_EL6900                      = 0x1AF43052,
    ecprodcode_beck_EL9800                      = 0x26483052,
    ecprodcode_beck_FM5001                      = 0x13893462,
    ecprodcode_beck_EK1110                      = 0x04562c52,
    ecprodcode_beck_EL2252                      = 0x08CC3052,
    ecprodcode_beck_EL1252                      = 0x04E43052,
    ecprodcode_beck_EL9820                      = 0x04570862,

    /* Ixxat */
    ecprodcode_ixx_iem                          = 0x00000001,
    ecprodcode_ixx_ETCio100                     = 0x00000006,
    
    /* ESR Pollmeier */
    ecprodcode_esr_Trio                         = 0x00001A63,
    ecprodcode_esr_Midi                         = 0x00001A59,
    ecprodcode_esr_Trio_1                       = 0x67550000,    /* may not work, ESR faulty Device Desc XML ? */
    
    /* KUEBLER */
    ecprodcode_kuebler_Multiturn5868            = 0x5868B111,
    
    /* KEB */
    ecprodcode_keb_KEB1736F5_3d                 = 0x000006c8,
    ecprodcode_keb_EcatGateway                  = 0x00002201,
    
    /* MOOG */
    ecprodcode_moog_anin                        = 0x00000001,
    ecprodcode_moog_ServoValveD671              = 0x000000D5,
                
    /* Lenze */
    ecprodcode_ldc_el9400                       = 0x00000001,
    ecprodcode_ldc_e94aycet                     = 0x00000002,
    ecprodcode_ldc_servogun2                    = 0x00000052,
    ecprodcode_ldc_servogun3                    = 0x00000053,
    ecprodcode_ldc_epms130                      = 0x00000514,
    ecprodcode_ldc_el8400                       = 0x000CD156,
    ecprodcode_ldc_stateline                    = 0x02010000,
    ecprodcode_ldc_highlineCIA402               = 0x38079D3D,
    
    /* Hilscher */
    ecprodcode_hil_NXSB100                      = 0x00000005,
    ecprodcode_hil_NXSB100DC                    = 0x7874656E,

    /* Schmidhauser */
    ecprodcode_sh_ACSE                          = 0x000000e0,
    
    /* SEW EuroDrive */
    ecprodcode_sew_movidrive                    = 0x00000001,
    ecprodcode_sew_moviaxis                     = 0x00000003,
    
    /* Danaher */
    ecprodcode_dan_servostar300                 = 0x00000000,
    
    /* Control Techniques */
    ecprodcode_ct_drive                         = 0x000001a5,
    ecprodcode_ct_drive_sp                      = 0x00030003,

    /* SMC */
    ecprodcode_smc_serifcunit                   = 0x22C53452,
    
    /* JUMO-Module */
    ecprodcode_jumo_Basis                       = 0x01010101,
    ecprodcode_jumo_Busskoppler                 = 0x02020202,
    ecprodcode_jumo_HMI                         = 0x03030303,
    ecprodcode_jumo_Router                      = 0x01234567,
    ecprodcode_jumo_BIO                         = 0x17214990,
    ecprodcode_jumo_RELAIS                      = 0x02345678,
    ecprodcode_jumo_REGLER                      = 0x26483053,
    ecprodcode_jumo_Analog_4_Ein                = 0x11111111,
    ecprodcode_jumo_Analog_8_Ein                = 0x22222222,
    ecprodcode_jumo_Analog_4_Aus                = 0x33333333,
    ecprodcode_jumo_Analog_8_Aus                = 0x44444444,
    
    /* BAUMUELLER */
    ecprodcode_baumueller_BM4000                = 0x00002775,
    ecprodcode_baumueller_BM3000                = 0x03010001,
    ecprodcode_baumueller_BM5000                = 0x05010100,
    
    /* Deutschmann */
    ecprodcode_dm_rs232gw                       = 0x26483052,
    
    /* Parker */
    ecprodcode_par_drive                        = 0x002E3BF1,
    
    /* National Instruments */
    ecprodcode_ni_digio                         = 0x0000000A,
    ecprodcode_ni_anaio                         = 0x000023b8,
    
    /* IDAM */
    ecprodcode_idam_DSMRW                       = 0x00000002,
    
    /* TR */
    ecprodcode_tr_linencoder2                   = 0x04D2AB01,
    
    /* BCE */
    ecprodcode_bce_AMAT_Handbox                 = 0x00000010,
    ecprodcode_bce_AMAT_HB_digout               = 0x00000020,
    ecprodcode_bce_AMAT_HB_digin                = 0x00000030,

    /* Koenig */
    ecprodcode_kng_pc104                        = 0x02628175,

    /* acontis */
    ecprodcode_at_atem                          = 0x6d657461,
    ecprodcode_at_atemTestSlave                 = 0xE0000001,

        
    /* KUKA */
    ecprodcode_kr_cib                           = 0x00000001,
    ecprodcode_kr_rdc3                          = 0x00000030,
    ecprodcode_kr_cibsion                       = 0x00000101,
    ecprodcode_kr_sionkpp                       = 0x00000110,
    ecprodcode_kr_sionksp                       = 0x00000111,

    /* Kuhnke */
    ecprodcode_kuh_VFIO_BK                      = 0x000248EC,
    ecprodcode_kuh_VFIO_DIO                     = 0x000248ED,

    /* JAT */
    ecprodcode_jat_drive1                       = 0x00000001,
    
    /* Festo */
    ecprodcode_fst_cpx                          = 0x00000026,
    
    /* Copley */
    ecprodcode_cpl_accelnet                     = 0x00000380,

    /* Robox */
    ecprodcode_rx_coedrivegw                    = 0x00000034,
    
    /* dresden elektronik */
    ecprodcode_de_sdac3100                      = 0x00000287,

    /* yaskawa */
    ecprodcode_yas_sgdv_e1                      = 0x02200001, 

    /* metronix */
    ecprodcode_metronix_ARS2000                 = 0x264860D3, 

    /* vipa */
    ecprodcode_vipa_coupler_053                 = 0x0531EC00, 

    /* hitachi */ /* add */
    ecprodcode_hitachi_Slave                    = 0x01010100, 

    /* OMRON */ /* add */
    ecprodcode_omron_Slave                      = 0x00000001, 

    /* MAXON */ /* add */
    ecprodcode_maxon_EPOS3						= 0x64400000, 

    /* TAMAGAWA */ /* add */
     ecprodcode_tamagawa_TA8499  				= 0x00002133, 

    /* Softservo */ /*added*/
    ecprodcode_YNL_MD4KW_3M  				= 0x00000005, 
    ecprodcode_YNL_MD4KW_2MFS  				= 0x00000006, 
    ecprodcode_YNL_MD4KW_Hand  				= 0x00000007, 
    ecprodcode_YNL_MD4KW_IMU  				= 0x00000008, 

    /* Borland C++ datatype alignment correction */
    ecprodcode_BCppDummy                        = 0xFFFFFFFF
} T_eEtherCAT_ProductCode;


/*-DEFINES-------------------------------------------------------------------*/
#define IGNORE_SERIAL ((EC_T_DWORD)(-1))


/*-FORWARD DECLARATIONS------------------------------------------------------*/


#endif /* __SLAVEINFO_H__ */

/*-END OF SOURCE FILE--------------------------------------------------------*/
