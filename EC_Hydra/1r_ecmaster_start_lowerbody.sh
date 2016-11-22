#!/bin/bash

# 7$B<4!"%a%G%#%"%3%s%P!<%?(B1$B$D7PM3(B
#./ECwork/DCDemo -f ~/ECMaster/ENI/hydra20150421.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
# ./ECwork/DCDemo -f ./ENI/hydra20150511.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
#./ECHydra/DCDemo -f ./ENI/hydra_no_hands_20150723.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
# ./ECHydra/DCDemo -f ./ENI/hydra_rhand_only_20150803.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 -nodcm -perf 
#./DCDemo -f ../../ENI/hydra_wholebody_20150625.xml -b 1000 -a 0 -t 0 -v 0 -i8254x 1 1 -nodcm  
#./ECwork/DCDemo -f ~/ethercat/ENI/ET9000_TAMAx2_CU1561_SYNC_1000.xml -b 1000 -a 0 -t 0 -v 10 -i8254x 1 1 


##########################
#$B3F<o(BOPTION$B$NCM(B
DURATION=0						# $B<B9T;~4V(B(0:$BL5@)8B!"(B1~:ms$B;~4V7P2a8e=*N;(B)
BUSCYCLE=1000					# $B<~4|<B9T;~4V(B($BC10L!'(Bus)
CPU_SEL=0						# DCDemo$B$N(BCPU$B%3%"(B No.$B;XDj(B
VERBOSE=10						# $B>pJsI=<(%l%Y%k;XDj(B
NETPORT=1						# $B;HMQ$9$k%M%C%H%o!<%/%]!<%HHV9f(B

# $B3F<o(BPATH$B@_Dj(B
# $B%m%0%U%!%$%k$N3JG<>l=j(B
LOG_PATH=$HOME/log				
LOG_FILE=$LOG_PATH/ec-master	#$B=PNO$9$k%m%0%U%!%$%k$N;XDj(B

# $B%M%C%H%o!<%/9=@.%U%!%$%k(B(ENI)$B$N;XDj(B
#ENI_PATH=$HOME/ethercat/ECwork_Hydra/ENI
ENI_PATH=../ENI
#ENI_FILE=$ENI_PATH/hydra_wholebody_20150625.xml
#ENI_FILE=$ENI_PATH/hydra_wholebody_20160108_SM.xml
#ENI_FILE=$ENI_PATH/hydra_wholebody_20160115_SM.xml
#ENI_FILE=$ENI_PATH/hydra_upperbody_20161031_SM.xml
#ENI_FILE=$ENI_PATH/hydra_lowerbody_20161031_SM.xml
ENI_FILE=$ENI_PATH/hydra_lowerbodywithoutIMU_20161122_SM.xml


# $B5/F0;~$N%Q%i%a!<%?@_Dj%U%!%$%k$N;XDj(B($BA*Br<0(B)
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
		# $BA*Br$7$?@_Dj%U%!%$%k$r<!2sA*Br;~$K$bI=<($5$;$k(B
		echo $PARAM_FILE > .env_pre_paramfile
		PARAM_FILE=$PARAM_PATH/$PARAM_FILE
		break
	fi
done
echo selected $REPLY\) $PARAM_FILE
echo ""


#$B3F<o(BOPTION$B;XDj(B
OP_F="-f ${ENI_FILE}"
#$B3F<o(BOPTION$B;XDj(B
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
OP_N="-i8254x ${NETPORT} 1"	# $B%M%C%H%o!<%/%G%P%$%9;XDj(B

# $BFC<l@_Dj(B
#OP_P="-perf"				# $B;~4V7WB,MQ0z?t(B
OP_P=""
OP_DCM="-nodcm"				# DCM$BL58z2=(B
#OP_P=""

##########################
# $B%W%m%;%9$,$b$7;D$C$F$$$?$iDd;_$5$;$k(B
echo "killall process..."
sudo killall --wait --quiet --regexp "DCDemo" 

echo "> ./HydraECAT ${OP_F} ${OP_S} ${OP_L} ${OP_T} ${OP_B} ${OP_A} ${OP_V} ${OP_P} ${OP_N} ${OP_DCM}"
sudo -E ./HydraECAT ${OP_F} ${OP_S} ${OP_L} ${OP_T} ${OP_B} ${OP_A} ${OP_V} ${OP_P} ${OP_N} ${OP_DCM}

exit 0
