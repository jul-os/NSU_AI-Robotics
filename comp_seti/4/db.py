import config

from psycopg2 import connect
from psycopg2.extras import execute_values


class Database:
    def __init__(self):
        self.connection = connect(
            database="news",
            user=config.DB_USER,
            password=config.DB_PASSWORD,
            host=config.DB_HOST,
            port=config.DB_PORT,
        )

        self.cursor = self.connection.cursor()

    def get_all_data(self) -> list:
        self.cursor.execute("SELECT * FROM news_data")
        return self.cursor.fetchall()

    def save_to_db(self, data_list: list):
        if not data_list:
            print("Нет данных для сохранения в базу данных.")
            return

        insert_query = """
        INSERT INTO news_data (url, date, time, title, author, parsed_at)
        VALUES %s
        ON CONFLICT (url) DO NOTHING
        """
        values = [
            (
                item["url"],
                item["date"],
                item["time"],
                item["title"],
                item["author"],
                item["parsed_at"],
            )
            for item in data_list
        ]

        try:
            execute_values(self.cursor, insert_query, values)
            self.connection.commit()
            print(f"Успешно сохранено {len(values)} записей в базу данных.")
        except Exception as e:
            self.connection.rollback()
            print(f"Ошибка при сохранении в базу данных: {e}")

    def clean_db(self):
        try:
            self.cursor.execute("DELETE FROM news_data")
            self.connection.commit()
            print("База данных очищена.")
        except Exception as e:
            self.connection.rollback()
            print(f"Ошибка при очистке базы данных: {e}")

    def close(self):
        self.cursor.close()
        self.connection.close()
