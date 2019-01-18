#!/bin/sh



print_status()
{
        script=$(basename $0)
        if [ "$STATUS" = 0 ]; then
                printf "$script: Exiting PASS\n\n"
        else
                printf "$script: Exiting FAIL\n\n"
        fi
}


check_devnode()
{
        script=$(basename $0)
        echo "Checking for devnode: $1"
        if [ ! -e $1 ]; then
                STATUS=1
                printf "$script: FAIL devnode not found: $1\n\n"
        else
                printf "$script: PASS devnode found: $1\n\n"
        fi

        # sleep a little to let autotest.pl script catch up with the logging.
        sleep 2
}

