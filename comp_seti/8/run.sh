#!/bin/bash
set -e

PROJECT="news"
DB_USER="news_user"
DB_PASS="secure_password_123"
DB_NAME="news"
APP_HOST_PORT=8080
NGINX_HOST_PORT=8081

echo "Запуск деплоя проекта: $PROJECT"

docker stop "${PROJECT}-db" "${PROJECT}-app" 2>/dev/null || true
docker rm "${PROJECT}-db" "${PROJECT}-app" 2>/dev/null || true

docker network create "${PROJECT}-net" 2>/dev/null || true
docker volume create "${PROJECT}-data" 2>/dev/null || true

sudo mkdir -p /etc/nginx/conf.d/geo_lists

if curl -sf https://www.ipdeny.com/ipblocks/data/aggregated/ru-aggregated.zone -o /tmp/ru_ips.zone; then
    {
        echo "default 0;"
        awk '{print $1" 1;"}' /tmp/ru_ips.zone
    } | sudo tee /etc/nginx/conf.d/geo_lists/ru_ips.conf > /dev/null
else
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

sudo tee /etc/nginx/conf.d/${PROJECT}-proxy.conf > /dev/null << EOF
geo \$http_x_forwarded_for \$is_blocked {
    include /etc/nginx/conf.d/geo_lists/ru_ips.conf;
}

server {
    listen ${NGINX_HOST_PORT} default_server;
    listen [::]:${NGINX_HOST_PORT} default_server;
    server_name _;

    location / {
        if (\$is_blocked = 1) {
            return 200 '<!DOCTYPE html><html><head><meta charset="utf-8"><title>Доступ ограничен</title><style>body{font-family:sans-serif;text-align:center;padding:80px;background:#f5f5f5}h1{color:#c00;font-size:2em}p{color:#666}</style></head><body><h1>Access denied</h1><p>Access from your region is restricted.</p></body></html>';
        }
        proxy_pass http://127.0.0.1:${APP_HOST_PORT};
        proxy_set_header Host \$host;
        proxy_set_header X-Real-IP \$remote_addr;
        proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto \$scheme;
    }
}
EOF

sudo nginx -t && sudo systemctl reload nginx
sudo setsebool -P httpd_can_network_connect 1 2>/dev/null || true
sudo firewall-cmd --permanent --add-port=${NGINX_HOST_PORT}/tcp 2>/dev/null && sudo firewall-cmd --reload 2>/dev/null || true

docker run -d \
  --name "${PROJECT}-db" \
  --network "${PROJECT}-net" \
  -e POSTGRES_USER="$DB_USER" \
  -e POSTGRES_PASSWORD="$DB_PASS" \
  -e POSTGRES_DB="$DB_NAME" \
  -v "${PROJECT}-data:/var/lib/postgresql/data" \
  -v "$(pwd)/init.sql:/docker-entrypoint-initdb.d/init.sql:ro" \
  --restart unless-stopped \
  postgres:15-alpine

echo "Ожидание инициализации базы данных..."
for i in {1..20}; do
  docker exec "${PROJECT}-db" pg_isready -U "$DB_USER" -d "$DB_NAME" &>/dev/null && break
  sleep 1
done

docker build -t "${PROJECT}-image" .
docker run -d \
  --name "${PROJECT}-app" \
  --network "${PROJECT}-net" \
  -e DB_USER="$DB_USER" \
  -e DB_PASSWORD="$DB_PASS" \
  -e DB_HOST="${PROJECT}-db" \
  -e DB_PORT=5432 \
  -e DB_NAME="$DB_NAME" \
  -p "${APP_HOST_PORT}:8000" \
  --shm-size=2gb \
  --restart unless-stopped \
  "${PROJECT}-image"

echo "Приложение запущено"
echo "Доступ: http://213.176.64.47:8081"
echo "Документация: http://213.176.64.47:8081/docs"

