# main.py
import asyncio
import os
import uvicorn

from contextlib import asynccontextmanager
from fastapi import FastAPI

from parser import Parser
from db import Database

# Глобально — только БД
database = Database()
_parser = None  #  парсер создадим позже


def get_parser():
    global _parser
    if _parser is None:
        _parser = Parser(headless=True)
    return _parser


@asynccontextmanager
async def lifespan(app: FastAPI):
    yield
    await asyncio.to_thread(database.close)
    if _parser is not None:
        await asyncio.to_thread(_parser.close)


app = FastAPI(lifespan=lifespan)


@app.put("/parse")
def parse_and_save():
    try:
        parser = get_parser()  # создаётся только при вызове /parse
        data = parser.parse_site(max_news=20, output_file="ura_news_data.csv")
        database.save_to_db(data)
        return {"message": "Парсинг успешно завершён"}
    except Exception as e:
        return {"error": f"Ошибка при парсинге: {e}"}


@app.get("/data")
def get_data():
    try:
        data = database.get_all_data()
        return [
            {
                "url": item[0],
                "date": item[1],
                "time": item[2],
                "title": item[3],
                "author": item[4],
                "parsed_at": item[5],
            }
            for item in data
        ]

    except Exception as e:
        return {"error": f"Ошибка при получении данных из базы данных: {e}"}


@app.get("/clean_db")
def clean_db():
    database.clean_db()
    return {"status": "success"}


@app.put("/shutdown")
def shutdown_server():
    try:
        parser.close()
        database.close()
    except:
        pass

    print("Сервер завершает работу...")
    os._exit(0)


if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8000)
