#!/bin/bash

# 7軸、メディアコンバータ1つ経由
#./ECwork/DCDemo -f ~/ECMaster/ENI/hydra20150421.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
# ./ECwork/DCDemo -f ./ENI/hydra20150511.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
#./ECHydra/DCDemo -f ./ENI/hydra_no_hands_20150723.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
# ./ECHydra/DCDemo -f ./ENI/hydra_rhand_only_20150803.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
#./DCDemo -f ../../ENI/hydra_wholebody_20150625.xml -b 1000 -a 0 -t 0 -v 0 -i8254x 1 1 -nodcm  
#./ECwork/DCDemo -f ~/ethercat/ENI/ET9000_TAMAx2_CU1561_SYNC_1000.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 


##########################
#各種OPTIONの値
DURATION=0						# 実行時間(0:無制限、1~:ms時間経過後終了)
BUSCYCLE=1000					# 周期実行時間(単位：us)
CPU_SEL=0						# DCDemoのCPUコア No.指定
VERBOSE=10						# 情報表示レベル指定
NETPORT=1						# 使用するネットワークポート番号

# 各種PATH設定
# ログファイルの格納場所
LOG_PATH=$HOME/log				
LOG_FILE=$LOG_PATH/ec-master	#出力するログファイルの指定

# ネットワーク構成ファイル(ENI)の指定
#ENI_PATH=$HOME/ethercat/ECwork_Hydra/ENI
ENI_PATH=../ENI
#ENI_FILE=$ENI_PATH/hydra_wholebody_20150625.xml
#ENI_FILE=$ENI_PATH/hydra_wholebody_20160108_SM.xml
#ENI_FILE=$ENI_PATH/hydra_wholebody_20160115_SM.xml
#ENI_FILE=$ENI_PATH/hydra_upperbody_20161031_SM.xml
#ENI_FILE=$ENI_PATH/hydra_lowerbody_20161031_SM.xml
ENI_FILE=$ENI_PATH/hydra_lowerbodywithoutIMU_20161122_SM.xml


# 起動時のパラメータ設定ファイルの指定(選択式)
#PARAM_PATH=$HOME/ethercat/param
#PARAM_PATH=$HOME/ethercat/ECwork_Hydra/param
PARAM_PATH=$HOME/ethercat/ecmaster_integration/EC_Hydra

PF1=Gain_20V.csv
PF2=gain_tuning_0210.csv 
PF3=Gain_48V_d0.csv 

PF0=`cat .env_pre_paramfile`
PF0="-> [Last Choice : $PF0]"
PFx="-> [Create ParamFile(param_out.csv)]"

echo ""
echo "Please choose a Param file."
select PARAM_FILE in "$PF0" "$PFx" "$PF1" "$PF2" "$PF3"
do
	if [ -z "$PARAM_FILE" ]; then
		continue
	elif [ "$PARAM_FILE" = "$PF0" ]; then
		PARAM_FILE=`cat .env_pre_paramfile`
		PARAM_FILE=$PARAM_PATH/$PARAM_FILE
		break
	elif [ "$PARAM_FILE" = "$PFx" ]; then
		PARAM_FILE=""
		break
	else
		# 選択した設定ファイルを次回選択時にも表示させる
		echo $PARAM_FILE > .env_pre_paramfile
		PARAM_FILE=$PARAM_PATH/$PARAM_FILE
		break
	fi
done
echo selected $REPLY\) $PARAM_FILE
echo ""


#各種OPTION指定
OP_F="-f ${ENI_FILE}"
#各種OPTION指定
if [ -z "$PARAM_FILE" ]; then
  OP_S=""
else
  OP_S="-s ${PARAM_FILE}"
fi
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
