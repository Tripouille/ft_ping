#!/bin/bash
docker images -a | grep mi > /dev/null 2>&1 || docker build -t mi .
docker rm -f mc > /dev/null 2>&1
docker run -d -v $(pwd):/ft_ping --name mc mi
docker exec -ti -w /ft_ping mc zsh