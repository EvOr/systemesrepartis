#!/bin/bash
xterm -T Client1 -e client/client Client1 23456 &
sleep 1
xterm -T Client2 -e client/client Client2 23457 &
sleep 1
xterm -T Client3 -e client/client Client3 23458 &
