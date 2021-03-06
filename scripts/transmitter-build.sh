# RUN FROM ROOT DIRECTORY ex. ./scripts/<script>.sh
docker-compose -f docker/transmitter-compose.yml up -d --build
docker exec transmitter-env /bin/bash -c "make clobber"
docker exec transmitter-env /bin/bash -c "make flash"
docker-compose -f docker/transmitter-compose.yml down
docker system prune -f