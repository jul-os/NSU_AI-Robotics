#!/bin/bash

docker stop postgres_db  && \
docker rm postgres_db  && \
docker stop news-app   && \
docker rm news-app   && \
docker volume rm pgdata  && \
docker network rm app-network  && \

set -e
docker network create app-network 2>/dev/null || echo "Сеть уже существует"

sudo mkdir -p /etc/nginx/conf.d/geo_lists

if curl -sf https://www.ipdeny.com/ipblocks/data/aggregated/ru-aggregated.zone -o /tmp/ru_ips.zone; then
    echo "Список IP скачан, конвертируем в формат geo..."
    {
        echo "default 0;"
        awk '{print $1" 1;"}' /tmp/ru_ips.zone
    } | sudo tee /etc/nginx/conf.d/geo_lists/ru_ips.conf > /dev/null
else
    echo " Не удалось скачать список, используем минимальный ручной список"
    sudo tee /etc/nginx/conf.d/geo_lists/ru_ips.conf > /dev/null << 'MINI_LIST'
default 0;
5.3.0.0/16 1;
5.18.0.0/16 1;
31.13.64.0/19 1;
37.9.64.0/19 1;
46.17.40.0/22 1;
77.88.0.0/14 1;
84.201.128.0/18 1;
84.237.0.0/16 1;
87.250.224.0/19 1;
95.108.128.0/17 1;
178.154.128.0/17 1;
185.32.185.0/24 1;
199.36.240.0/22 1;
213.180.192.0/19 1;
MINI_LIST
fi

sudo tee /etc/nginx/conf.d/news-proxy.conf > /dev/null << 'EOF'
geo $http_x_forwarded_for $is_blocked {
    include /etc/nginx/conf.d/geo_lists/ru_ips.conf;
}

server {
    listen 80 default_server;
    listen [::]:80 default_server;
    server_name _;

    location / {
        if ($is_blocked = 1) {
            return 200 '<!DOCTYPE html><html><head><meta charset="utf-8"><title>Доступ ограничен</title><style>body{font-family:sans-serif;text-align:center;padding:80px;background:#f5f5f5}h1{color:#c00;font-size:2em}p{color:#666}</style></head><body><h1>⛔ ВАМ СЮДА НЕЛЬЗЯ</h1><p>Доступ с территории РФ ограничен.</p></body></html>';
        }
        proxy_pass http://127.0.0.1:8080;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
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
  -p 5433:5432 \
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