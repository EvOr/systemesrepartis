#!/bin/bash
#
# Start the Recd Daemon
# Author : JVi
#
pidFile=graou.pid


#********************************************************#
#        Fonction de test de la pr�sence du d�mon        #
#********************************************************#
# Ce sont les fonctions qui testent l'�tat du d�mon afin #
# d'�viter qu'il ne soit lanc� deux fois, ou d'effectuer #
# le traitement d'extinction alors que le d�mon n'est    # 
# pas lanc� 						 #
##########################################################

ifAlreadyLaunchedQuit(){
   portmap=`ps aux | grep portmap | grep -v grep`
   if [ -z "$portmap" ]; then
      echo -n "					[ FAILED ]"
      echo " "
      echo "Please launch portmap before the server"
      exit 1
   fi
   if [ -e $pidFile ]; then
      PIDToTest=`cat $pidFile`
      launchedProof=`ps aux | grep serverRPC | grep -v grep`
      if [ -n "$launchedProof" ]; then
	 echo -n "					[ FAILED ]"
	 echo " "
	 echo "Already Launched : if not please delete $pidFile"
	 exit 1
      fi
   fi
}

ifNotAlreadyLaunchedQuit(){
   if [ ! -e $pidFile ]; then
      echo -n "					[ FAILED ]"
      echo " "
      echo "The server is not currently launched"
      exit 1
   fi
}

#********************************************************#
#             Fonction de lancement du demon             #
#********************************************************#

start(){
   echo -n "Starting..."
   myPID=$$
   ifAlreadyLaunchedQuit
   touch $pidFile
   echo -n "					[ OK ]"
   echo " "
   server/serverRPC > /dev/null&
}

#********************************************************#
#                Fonction d'arret du demon               #
#********************************************************#

stop(){
   echo -n "Stopping..."
   ifNotAlreadyLaunchedQuit
   kill -2 `ps aux | grep "serverRPC" | grep -v $$ | cut -c10-15` 2> /dev/null
   echo -n "					[ OK ]"
   echo " "
   echo "Daemon Stopped successfully"
   rm $pidFile
}

#********************************************************#
#            Fonction de relancement du d�mon            #
#********************************************************#

restart(){
   stop
   start
}

#********************************************************#

#********************************************************#
case $1 in
   start)

   start
   ;;
   stop)
   stop
   ;;
   restart)
   restart
   ;;
   *)
   echo "Usage : start|stop|restart"
   exit 2
   ;;
esac
