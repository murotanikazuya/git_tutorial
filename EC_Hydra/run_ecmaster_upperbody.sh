#!/bin/bash


##########################
#$B3F<o(BOPTION$B$NCM(B
DURATION=0						# $B<B9T;~4V(B(0:$BL5@)8B!"(B1~:ms$B;~4V7P2a8e=*N;(B)
BUSCYCLE=1000					# $B<~4|<B9T;~4V(B($BC10L!'(Bus)
CPU_SEL=0						# DCDemo$B$N(BCPU$B%3%"(B No.$B;XDj(B
VERBOSE=10						# $B>pJsI=<(%l%Y%k;XDj(B
NETPORT=1						# $B;HMQ$9$k%M%C%H%o!<%/%]!<%HHV9f(B

# $B3F<o(BPATH$B@_Dj(B
# $B%m%0%U%!%$%k$N3JG<>l=j(B
LOG_PATH=./log
LOG_FILE=$LOG_PATH/ec-master	#$B=PNO$9$k%m%0%U%!%$%k$N;XDj(B

# $B%M%C%H%o!<%/9=@.%U%!%$%k(B(ENI)$B$N;XDj(B
ENI_PATH=../ENI

ENI_FILE=$ENI_PATH/upperbody.xml

OP_S="-s hydra_gains_upperbody.csv -upperbody"


#$B3F<o(BOPTION$B;XDj(B                                                                                                                     
OP_F="-f ${ENI_FILE}"


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
