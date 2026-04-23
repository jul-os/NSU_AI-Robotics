-- init.sql
-- Примечание: переменная POSTGRES_DB=news уже создаёт БД автоматически,
-- поэтому скрипт выполняется уже в контексте базы "news"

CREATE TABLE IF NOT EXISTS news_data (
    url TEXT PRIMARY KEY,
    date TEXT,
    time TEXT,
    title TEXT,
    author TEXT,
    parsed_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE INDEX IF NOT EXISTS idx_news_url ON news_data(url);
CREATE INDEX IF NOT EXISTS idx_news_parsed_at ON news_data(parsed_at);
