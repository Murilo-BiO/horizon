#!/bin/bash
###################################################
#       _   _            _                        #
#      | | | |          (_)                       #
#      | |_| | ___  _ __ _ _______  _ __          #
#      |  _  |/ _ \| '__| |_  / _ \| '_  \        #
#      | | | | (_) | |  | |/ / (_) | | | |        #
#      \_| |_/\___/|_|  |_/___\___/|_| |_|        #
###################################################
# This file is part of Horizon (c).
# Copyright (c) 2018 Horizon Dev Team.
#
# Base Author - Sagun Khosla. (sagunxp@gmail.com)
#
# Under a proprietary license this file is not for use
# or viewing without permission.
###################################################

MODE="$1"
shift


function header
{
    echo "                                                 "
    echo "       _   _            _                        "
    echo "      | | | |          (_)                       "
    echo "      | |_| | ___  _ __ _ _______  _ __          "
    echo "      |  _  |/ _ \| '__| |_  / _ \| '_  \        "
    echo "      | | | | (_) | |  | |/ / (_) | | | |        "
    echo "      \_| |_/\___/|_|  |_/___\___/|_| |_|        "
    echo "                                                 "
    echo "        A new dawn for ragnarok online.          "
    echo "                                                 "
}

function usage
{
    header
	echo "usage:"
	echo "    $0 start"
	echo "    $0 stop"
	echo "    $0 build [configure args]"
	echo "    $0 auth <dbname> [dbuser] [dbpassword] [dbhost] [args]"
	echo "    $0 char <dbname> [dbuser] [dbpassword] [dbhost] [args]"
	echo "    $0 test <dbname> [dbuser] [dbpassword] [dbhost]"
	echo "    $0 createdb <dbname> [dbuser] [dbpassword] [dbhost]"
	echo "    $0 importdb <dbname> [dbuser] [dbpassword] [dbhost]"
	echo "    $0 adduser <dbname> <new_user> <new_user_password> [dbuser] [dbpassword] [dbhost]"
	exit 1
}

function build_horizon
{
    echo "Horizon Build Initiated."
    echo "Import Complete."
    echo "Preparing Build Directory..."
    mkdir build
    cd build
    rm -rf ./*
    cmake ../ $@ || aborterror "Horizon Build has failed."
    echo "Initiating Build..."
    make install -j 2 || aborterror "Horizon Build has failed."
    echo "Build Complete."
    cd ../
}

function aborterror
{
	echo $@
	exit 1
}

function console_log
{
    echo "Horizon: $@"
}

DBNAME=horizon
DBUSER=ragnarok
DBPASS=ragnarok
DBHOST=localhost

case "$MODE" in
	createdb|importdb|test)
		if [ -z "$1" ]; then
			usage
		fi
		DBNAME="$1"
		if [ -n "$2" ]; then
			DBUSER_ARG="--user=$2"
			DBUSER="$2"
		fi
		if [ -n "$3" ]; then
			DBPASS_ARG="--password=$3"
			DBPASS="$3"
		fi
		if [ -n "$4" ]; then
			DBHOST_ARG="--host=$4"
			DBHOST="$4"
		fi
		;;
	adduser)
		if [ -z "$3" ]; then
			usage
		fi
		DBNAME="$1"
		NEWUSER="$2"
		NEWPASS="$3"
		if [ -n "$4" ]; then
			DBUSER_ARG="--user=$4"
			DBUSER="$4"
		fi
		if [ -n "$5" ]; then
			DBPASS_ARG="--password=$5"
			DBPASS="$5"
		fi
		if [ -n "$6" ]; then
			DBHOST_ARG="--host=$6"
			DBHOST="$6"
		fi
		;;
esac

case "$MODE" in
    start)
        if auth 2> /dev/null; then
            auth "$@"
        else
            if ./bin/auth 2> /dev/null; then
                ./bin/auth "$@"
            else
                aborterror "Auth server not compiled, please compile before running."
            fi
        fi
        ;;
	createdb)
		console_log "Creating Horizon MySQL Database $DBNAME as $DBUSER..."
		mysql $DBUSER_ARG $DBPASS_ARG $DBHOST_ARG --execute="CREATE DATABASE $DBNAME;" || aborterror "Unable to create database."
		;;
	importdb)
        console_log "Importing Horizon MySQL Tables into $DBNAME as $DBUSER..."
		mysql $DBUSER_ARG $DBPASS_ARG $DBHOST_ARG --database=$DBNAME < sql-files/horizon.sql || aborterror "Unable to import horizon database."
		;;
	adduser)
		console_log "Adding user $NEWUSER as $DBUSER, with access to database $DBNAME..."
		mysql $DBUSER_ARG $DBPASS_ARG $DBHOST_ARG --execute="GRANT SELECT, INSERT, UPDATE, DELETE ON $DBNAME.* TO '$NEWUSER'@'$DBHOST' IDENTIFIED BY '$NEWPASS';"
		;;
	build)
	    build_horizon $@
	    ;;
	*)
	usage
	;;
esac
