#!/bin/bash

java -jar server.jar > logs/server.log &
java -jar web.jar > logs/web.log &
java -jar aim.jar > logs/aim.log &
open applets/admin/index.html
