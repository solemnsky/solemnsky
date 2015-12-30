docker run -it -v /tmp/.X11-unix:/tmp/.X11-unix -v /home/:/home -e DISPLAY=unix$DISPLAY --name clion kurron/docker-clion
