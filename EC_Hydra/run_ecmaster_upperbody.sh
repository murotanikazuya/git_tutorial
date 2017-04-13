#!/bin/bash


##########################
#各種OPTIONの値
DURATION=0						# 実行時間(0:無制限、1~:ms時間経過後終了)
BUSCYCLE=1000					# 周期実行時間(単位：us)
CPU_SEL=0						# DCDemoのCPUコア No.指定
VERBOSE=10						# 情報表示レベル指定
NETPORT=1						# 使用するネットワークポート番号

# 各種PATH設定
# ログファイルの格納場所
LOG_PATH=./log
LOG_FILE=$LOG_PATH/ec-master	#出力するログファイルの指定

# ネットワーク構成ファイル(ENI)の指定
ENI_PATH=../ENI

ENI_FILE=$ENI_PATH/upperbody.xml

OP_S="-s hydra_gains_upperbody.csv -upperbody"


#各種OPTION指定                                                                                                                     
OP_F="-f ${ENI_FILE}"


OP_L="-log ${LOG_FILE}"

OP_B="-b ${BUSCYCLE}"
OP_A="-a ${CPU_SEL}"
OP_T="-t ${DURATION}"
OP_V="-v ${VERBOSE}"
OP_N="-i8254x ${NETPORT} 1"	# ネットワークデバイス指定

# 特殊設定
#OP_P="-perf"				# 時間計測用引数
OP_P=""
OP_DCM="-nodcm"				# DCM無効化
#OP_P=""

##########################
# プロセスがもし残っていたら停止させる
echo "killall process..."
sudo killall --wait --quiet --regexp "DCDemo" 

echo "> ./HydraECAT ${OP_F} ${OP_S} ${OP_L} ${OP_T} ${OP_B} ${OP_A} ${OP_V} ${OP_P} ${OP_N} ${OP_DCM}"
sudo -E ./HydraECAT ${OP_F} ${OP_S} ${OP_L} ${OP_T} ${OP_B} ${OP_A} ${OP_V} ${OP_P} ${OP_N} ${OP_DCM}

exit 0
