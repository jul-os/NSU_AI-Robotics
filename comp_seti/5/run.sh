#!/bin/bash

docker stop news-app postgres_db  && \
docker rm news-app postgres_db  && \

docker volume rm pgdata  && \

docker network rm app-network  && \


set -e
docker network create app-network 2>/dev/null || echo "Сеть уже существует"


docker run -d \
  --name postgres_db \
  --network app-network \
  -e POSTGRES_USER=news_user \
  -e POSTGRES_PASSWORD=secure_password_123 \
  -e POSTGRES_DB=news \
  -v pgdata:/var/lib/postgresql/data \
  -v "/home/julia/Рабочий стол/code/NSU_AI-Robotics/comp_seti/5/init.sql:/docker-entrypoint-initdb.d/init.sql" \
  -p 5432:5432 \
  postgres:15-alpine && \

sleep 5 && \
#docker exec postgres_db psql -U news_user -d news -c "\dt" && \ 

docker build -t news-parser-app . && \
docker run -d \
  --name news-app \
  --network app-network \
  -e DB_USER=news_user \
  -e DB_PASSWORD=secure_password_123 \
  -e DB_HOST=postgres_db \
  -e DB_PORT=5432 \
  -e DB_NAME=news \
  -p 8080:8000 \
  --shm-size=2gb \
  news-parser-app  && \

docker ps && \
docker logs -f news-app && \

echo "открой http://localhost:8080/docs"