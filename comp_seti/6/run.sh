#!/bin/bash

docker stop news-app postgres_db  && \
docker rm news-app postgres_db  && \

docker volume rm pgdata  && \

docker network rm app-network  && \


set -e
docker network create app-network 2>/dev/null || echo "Сеть уже существует"

sudo tee /etc/nginx/conf.d/news-proxy.conf > /dev/null << 'EOF'
server {
    listen 80 default_server;
    listen [::]:80 default_server;
    server_name _;
    location / {
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
    }
}
EOF

sudo mv /etc/nginx/conf.d/default.conf /etc/nginx/conf.d/default.conf.disabled 2>/dev/null || true
sudo nginx -t && sudo systemctl reload nginx
sudo setsebool -P httpd_can_network_connect 1 2>/dev/null || true
sudo firewall-cmd --permanent --add-service=http 2>/dev/null && sudo firewall-cmd --reload 2>/dev/null || true

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

sleep 10 && \

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
echo "http://localhost/docs"

